#include <err.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "ui.h"
#include "train.h"
#include "resolve.h"
#include "../utils/utils.h"

#define PATH_MAX 200

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
	(void)widget;
	(void)user_data;
	g_print("save\n");
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
		data->ui.image_pixbuf= image;

		if (err != NULL)
			g_print("%s\n", err->message);

		// hide info text
		gtk_widget_hide(GTK_WIDGET(data->ui.main_info_label));

		gtk_widget_set_opacity(GTK_WIDGET(data->ui.main_image), 1.f);
		gtk_image_set_from_pixbuf(data->ui.main_image, image);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), TRUE);
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
	print_layer(data->net.network, 3);
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
