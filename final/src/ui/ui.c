#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "ui.h"
#include "train.h"
#include "resolve.h"
#include "../preprocess/gauss.h"
#include "../preprocess/gamma.h"
#include "../utils/utils.h"

#define PATH_MAX 200

#define N_LAYERS 3
#define N_NEURONS_INPUT 784
#define N_NEURONS_HIDDEN_1 32
#define N_NEURONS_OUTPUT 10

int file_exists2(const char *filename) {
    FILE *fp = fopen(filename, "r");
    int is_exist = 0;
    if (fp != NULL) {
        is_exist = 1;
        fclose(fp); // close the file
    }
    return is_exist;
}

// Callback of configure-event from window. It resize main image
gboolean on_configure_window(GtkWindow *window, GdkEvent *event, gpointer user_data) {
	(void)event;
	ProgramData *data = user_data;

	gint width = gtk_widget_get_allocated_width(GTK_WIDGET(window));
	gint height = gtk_widget_get_allocated_height(GTK_WIDGET(window));

	if (data->ui.image_pixbuf != NULL) {
		gint old_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
		gint old_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);
		gfloat ratio = (float)old_height / (float)old_width;
		gfloat new_height = (width - 500) * ratio;
		if ((width - 500) != old_width) {
			if (new_height < height - 200) {
				gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(data->ui.image_pixbuf, width - 500, (width - 500) * ratio, GDK_INTERP_BILINEAR));
			}
		}
	}
	return FALSE;
}


gboolean on_save_button(GtkWidget *widget, gpointer user_data) {
	ProgramData *data = user_data;
	(void)widget;
    FILE* stream = NULL;
	int i = 10;
	char *save_path;
	char *path = "./images/sudoku_solved";
	do {
		char* buf = (char *)calloc((int)(log10(i)) + 1, sizeof(char));
		snprintf(buf, (int)log10(i) + 1, "%i", i);
		save_path = (char *)calloc(strlen(buf) + 60, sizeof(char));
		strcat(save_path, path);
		strcat(save_path, buf);
		strcat(save_path, ".jpg");
		free(buf);
		i += 10;
	}
	while (file_exists2(save_path) == 1 && i < 1000); // TODO: put 1000 in a variable
	stream = fopen(save_path, "w");
	if (stream == NULL) {
		g_print("on_save_button(): Cannot access to stream\n");
	}
	else {
		IMG_SaveJPG(data->img.solved_surface, save_path, 100);
		g_print("on_save_button(): Saved in %s\n", save_path);
	}
	return FALSE;
}

// Callback of clicked event from open button
// Set main image
gboolean on_open_button(GtkWidget *widget, gpointer user_data) {
	(void)widget;
	ProgramData *data = user_data;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new (
			"Open File",
			data->ui.window,
			action,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);

		gtk_header_bar_set_subtitle(data->ui.header_bar, filename);
		GError *err = NULL;

		//gtk_image_set_from_file(ui->main_image, filename);
		// Update image path for resolution
		data->img.opened_image_path = filename;
		// Update pixbuf
		gint width = gtk_widget_get_allocated_width(GTK_WIDGET(data->ui.window));
		GdkPixbuf* image = gdk_pixbuf_new_from_file_at_scale (filename, width - 500, width - 500, TRUE, &err);
		//ui->main_image = GTK_IMAGE(gtk_image_new_from_file(filename));
		//GdkPixbuf* image = gdk_pixbuf_new_from_file_at_scale (filename, 600, 600, TRUE, &err);
		data->ui.image_pixbuf = image;
		data->img.original_pixbuf = image;
		data->img.current_pixbuf = image;

		if (err != NULL)
			g_print("%s\n", err->message);
		// hide info text
		gtk_widget_hide(GTK_WIDGET(data->ui.main_info_label));

		gtk_widget_set_opacity(GTK_WIDGET(data->ui.main_image), 1.f);
		gtk_image_set_from_pixbuf(data->ui.main_image, image);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), TRUE);

		// set filter btn
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.gauss_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.gamma_btn), TRUE);

		SDL_Surface* loaded_surface = IMG_Load(filename);

		gtk_revealer_set_reveal_child(data->ui.filters_revealer, TRUE);

		data->img.original_surface = loaded_surface;
		data->img.filtered_surface = SDL_ConvertSurfaceFormat(loaded_surface, SDL_PIXELFORMAT_ARGB8888, 0);
	}

	gtk_widget_destroy (dialog);
	return FALSE;
}

// Callback of clicked event from generate button
// Generate new 3 layers network (by randomize neurones of loaded network)
gboolean on_nn_generate_btn(GtkWidget* widget, gpointer user_data) {
	(void)widget;
	ProgramData *data = user_data;

	// Reset each neurons weights and biais
	// Generate network
	size_t N_NEURONS_ARRAY[] = {N_NEURONS_INPUT, N_NEURONS_HIDDEN_1, N_NEURONS_OUTPUT};
	size_t sizes_inputs[] = {784, 784, 32};

	Layer network[N_LAYERS];
	generate_network(network, N_LAYERS, N_NEURONS_ARRAY, sizes_inputs);
	/*
	   Layer *network = data->net.network;
	   for(size_t i = 0; i < data->net.layers; ++i) {
	   for(size_t j = 0; j < data->net.sizes_neurons[i]; ++j) {
	   for (double* k = network[i].neurons[j].begin; k < network[i].neurons[j].end; ++k)
	 *k = getRandom();
	 network[i].neurons[j].bias = getRandom();
	 network[i].neurons[j].value = 0;
	 network[i].neurons[j].delta = 0;
	 }
	 }
	 */

	gtk_widget_hide(GTK_WIDGET(data->ui.nn_info_bar));
	gtk_label_set_text(data->ui.nn_info_bar_label, "Network successfully generated (3 layers).");
	gtk_info_bar_set_message_type(data->ui.nn_info_bar, GTK_MESSAGE_INFO);
	gtk_widget_show(GTK_WIDGET(data->ui.nn_info_bar));

	gtk_label_set_text(data->ui.current_nn_path_label, "Internal Network");
	gtk_label_set_text(data->ui.current_nn_main_path_label, "Internal Network");

	return FALSE;
}

// Save the path of the network to load
gboolean on_nn_load_file_set(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	char *filename;
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);
	filename = gtk_file_chooser_get_filename(chooser);
	data->net.network_to_load_path =  filename;
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.nn_load_btn), TRUE);
	return FALSE;
}

// Callback of clicked event from load button.
// Load network
gboolean on_nn_load_btn(GtkWidget* widget, gpointer user_data) {
	(void)widget;
	ProgramData *data = user_data;

	printf("on_nn_load_btn(): %s\n", data->net.network_to_load_path);
	int err = load_weights(data->net.network_to_load_path, data->net.network);
	//print_layer(data->net.network, 3);
	if (err == 1)
		errx(1, "on_nn_load_btn: Failed to load network.\n");

	gtk_widget_hide(GTK_WIDGET(data->ui.nn_info_bar));
	gtk_label_set_text(data->ui.nn_info_bar_label, "Network successfully loaded.");
	gtk_info_bar_set_message_type(data->ui.nn_info_bar, GTK_MESSAGE_INFO);
	gtk_widget_show(GTK_WIDGET(data->ui.nn_info_bar));
	gtk_label_set_text(data->ui.current_nn_path_label, data->net.network_to_load_path);
	gtk_label_set_text(data->ui.current_nn_main_path_label, data->net.network_to_load_path);
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.nn_load_btn), FALSE);
	gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(data->ui.nn_file_chooser_btn));
	return FALSE;
}

// Callback of clicked event from export button
// Save network
gboolean on_nn_export_btn(GtkWidget* widget, gpointer user_data) {
	(void)widget;
	ProgramData *data = user_data;
	char *path = "./networks/exported_network_";
	char *save_path;
	int err = save_weights(path, data->net.network, data->net.layers, &save_path);
	if (err == 1)
		errx(1, "on_nn_export_btn: Failed to export network.\n");

	char res_path[PATH_MAX] = {0};
	strcat(res_path, "Network successfully exported at:  ");
	strcat(res_path, save_path);
	free(save_path);

	gtk_label_set_text(data->ui.nn_info_bar_label, res_path);

	gtk_info_bar_set_message_type(data->ui.nn_info_bar, GTK_MESSAGE_INFO);
	gtk_widget_hide(GTK_WIDGET(data->ui.nn_info_bar));
	gtk_widget_show(GTK_WIDGET(data->ui.nn_info_bar));
	return FALSE;
}
// Save the path of the dataset to load
gboolean on_train_file_set(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	char *foldername;
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);
	foldername = gtk_file_chooser_get_filename(chooser);
	data->net.dataset_path = foldername;
	return FALSE;
}

// Callback of clicked event from epochs spin button
gboolean on_epochs_spin_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData* data = user_data;
	data->net.epochs = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	return FALSE;
}

// Callback of clicked event from learning_rate spin button
gboolean on_lr_spin_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData* data = user_data;
	data->net.learning_rate = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	return FALSE;
}

// Callback original btn
gboolean on_original_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData* data = user_data;
	gint original_width = gdk_pixbuf_get_width(data->img.original_pixbuf);
	gint original_height = gdk_pixbuf_get_height(data->img.original_pixbuf);
	gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(data->img.original_pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
	data->img.filtered_surface = data->img.original_surface;
	// update filter index
	data->img.applied_filter = 0;

	// disable original btn
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), FALSE);
	// enable resolve btn / gauss and gamma filters
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), TRUE);
	gtk_toggle_button_set_active(data->ui.gauss_btn, FALSE);
	gtk_toggle_button_set_active(data->ui.gamma_btn, FALSE);
	// disable save btn
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.save_button), FALSE);
	return FALSE;
}

// Callback gauss btn
gboolean on_gauss_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData* data = user_data;
	// update current pixbuf
	//SDL_Surface* gauss_surface = SDL_ConvertSurfaceFormat(data->img.filtered_surface, SDL_PIXELFORMAT_ARGB8888, 0);
	//gauss_surface = blur(gauss_surface, 3); // TODO: change param 3
	SDL_Surface* gauss_surface;
	if (gtk_toggle_button_get_active(data->ui.gauss_btn)) {
		if (data->img.applied_filter == 0) {
			gauss_surface = SDL_ConvertSurfaceFormat(data->img.original_surface, SDL_PIXELFORMAT_ARGB8888, 0);
			gauss_surface = blur(gauss_surface, 3); // TODO: change param 3
			data->img.gauss_surface = gauss_surface;
			data->img.applied_filter = 1;
		}	
		else {
			gauss_surface = SDL_ConvertSurfaceFormat(data->img.filtered_surface, SDL_PIXELFORMAT_ARGB8888, 0);
			gauss_surface = blur(gauss_surface, 3); // TODO: change param 3
			data->img.applied_filter = 3;
		}
		data->img.filtered_surface = gauss_surface;

		// udpate ui
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), TRUE);
	}
	else {
		if (data->img.applied_filter == 3) {
			data->img.applied_filter = 2;
			data->img.filtered_surface = data->img.gamma_surface;
		}
		else {
			data->img.applied_filter = 0;
			data->img.filtered_surface = data->img.original_surface;
			gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), FALSE);
		}
	}

	GdkPixbuf* gauss_pixbuf = gtk_image_new_from_sdl_surface(data->img.filtered_surface);
	gint original_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
	gint original_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);

	//data->ui.image_pixbuf = pixbuf; 
	gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(gauss_pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
	//SDL_FreeSurface(gauss_surface);
	data->img.current_pixbuf = gauss_pixbuf;
	return FALSE;
}

// Callback gamma btn
gboolean on_gamma_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData* data = user_data;
	SDL_Surface* gamma_surface;
	if (gtk_toggle_button_get_active(data->ui.gamma_btn)) {
		if (data->img.applied_filter == 0) {
			gamma_surface = SDL_ConvertSurfaceFormat(data->img.original_surface, SDL_PIXELFORMAT_ARGB8888, 0);
			filter_image(gamma_surface, 1.2);
			data->img.gamma_surface = gamma_surface;
			data->img.applied_filter = 2;
		}	
		else {
			gamma_surface = SDL_ConvertSurfaceFormat(data->img.filtered_surface, SDL_PIXELFORMAT_ARGB8888, 0);
			filter_image(gamma_surface, 1.2);
			data->img.applied_filter = 3;
		}
		data->img.filtered_surface = gamma_surface;

		// udpate ui
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), TRUE);
	}
	else {
		if (data->img.applied_filter == 3) {
			data->img.applied_filter = 1;
			data->img.filtered_surface = data->img.gauss_surface;
		}
		else {
			data->img.applied_filter = 0;
			data->img.filtered_surface = data->img.original_surface;
			gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), FALSE);
		}
	}

	GdkPixbuf* gamma_pixbuf = gtk_image_new_from_sdl_surface(data->img.filtered_surface);
	gint original_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
	gint original_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);

	gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(gamma_pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
	data->img.current_pixbuf = gamma_pixbuf;
	return FALSE;
}
