//#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "src/utils/utils.h"
#include "src/network/network.h"
#include "src/network/training.h"

#define N_LAYERS 3
#define N_NEURONS_INPUT 784
#define N_NEURONS_HIDDEN_1 32
#define N_NEURONS_OUTPUT 10

#define DEFAULT_EPOCHS 20
#define DEFAULT_LR 0.1

void init_ocr(const char *network_path, Layer *network, GtkLabel* main_nn_path_label, GtkLabel* nn_nn_path_label) {
	int err = load_weights(network_path, network);
	if (err == 1)
		errx(1, "init_ocr: Failed to init ocr.\n");
	char cwd[PATH_MAX];
	char text[PATH_MAX] = {0};
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		strcat(text, cwd);
		strcat(text, "/");
		strcat(text, network_path);
	}
	else
		printf("init_ocr: Failed to get cwd.\n");
	gtk_label_set_label(main_nn_path_label, text);
	gtk_label_set_label(nn_nn_path_label, text);
}

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
		gint width = gtk_widget_get_allocated_width(GTK_WIDGET(data->ui.window));
		GdkPixbuf* image = gdk_pixbuf_new_from_file_at_scale (filename, width - 300, width - 300, TRUE, &err);
		//ui->main_image = GTK_IMAGE(gtk_image_new_from_file(filename));
		//GdkPixbuf* image = gdk_pixbuf_new_from_file_at_scale (filename, 600, 600, TRUE, &err);
		data->ui.image_pixbuf= image;

		if (err != NULL) {
			g_print("%s\n", err->message);
		}

		gtk_widget_set_opacity(GTK_WIDGET(data->ui.main_image), 1.f);
		gtk_image_set_from_pixbuf(data->ui.main_image, image);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), TRUE);
	}

	gtk_widget_destroy (dialog);
	return FALSE;
}

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

	gtk_label_set_label(data->ui.current_nn_path_label, "Internal Network");
	gtk_label_set_label(data->ui.current_nn_main_path_label, "Internal Network");

	return FALSE;
}

gboolean close_info_bar(GtkWidget* widget, gpointer bar) {
	(void)widget;
	gtk_widget_hide(widget);
	gtk_widget_hide(GTK_WIDGET(bar));
	return FALSE;
}

gboolean on_nn_load_btn(GtkWidget* widget, gpointer user_data) {
	(void)widget;
	ProgramData *data = user_data;
	int err = load_weights(data->net.network_to_load_path, data->net.network);
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

gboolean on_nn_load_file_set(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	char *filename;
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);
	filename = gtk_file_chooser_get_filename(chooser);
	data->net.network_to_load_path =  filename;
	gtk_widget_set_sensitive(GTK_WIDGET(data->ui.nn_load_btn), TRUE);
	return FALSE;
}

gboolean on_train_file_set(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	char *foldername;
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);
	foldername = gtk_file_chooser_get_filename(chooser);
	data->net.dataset_path = foldername;
	return FALSE;
}

gpointer launch_training(gpointer user_data) {
	ProgramData *data = user_data;

	/*
	for(int i = 0; i < 500000; i++) {
		if (data->net.is_training == 0 || !gtk_widget_get_visible(GTK_WIDGET(data->ui.nn_dialog))) {
			data->net.is_training = 0;
			data->net.training_retval = 0; // Stop before to finish
			g_print("not finished\n");
			g_thread_exit(NULL);
		}
		g_print("training: %i\n", i);
	}
	*/

	training(data->net.network, data->net.dataset_path, data->net.epochs, data->net.learning_rate, 1, user_data);

	g_print("finished\n");
	data->net.is_training = 0;
	data->net.training_retval = 1; // Stop normally
	g_thread_exit(NULL);
	return NULL;
}

gboolean update_ui_when_training(gpointer user_data) {
	ProgramData *data = user_data;
	if (data->net.is_training == 0) {
		gtk_button_set_label(data->ui.nn_train_btn, "Launch Training");
		return FALSE;
	}
	return TRUE;
}

gboolean on_nn_train_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	if (data->net.is_training) {
		g_print("Stop Training\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Launch Training");
		data->net.is_training = 0;
	}
	else {
		if (data->net.dataset_path == NULL) {
			gtk_info_bar_set_message_type(data->ui.nn_info_bar, GTK_MESSAGE_WARNING);
			gtk_widget_hide(GTK_WIDGET(data->ui.nn_info_bar));
			gtk_label_set_text(data->ui.nn_info_bar_label, "Please select a valid dataset.");
			gtk_widget_show(GTK_WIDGET(data->ui.nn_info_bar));
		}
		else {
			g_print("Start Training\n");
			gtk_button_set_label(GTK_BUTTON(widget), "Stop Training");
			data->net.is_training = 1;
			g_thread_new(NULL, launch_training, user_data);
			g_timeout_add(500, update_ui_when_training, user_data);
		}
	}
	return FALSE;
}

gboolean on_configure_window(GtkWindow *window, GdkEvent *event, gpointer user_data) {
	(void)event;
	ProgramData *data = user_data;

	gint width = gtk_widget_get_allocated_width(GTK_WIDGET(window));
	gint height = gtk_widget_get_allocated_height(GTK_WIDGET(window));

	if (data->ui.image_pixbuf != NULL) {
		gint old_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
		gint old_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);
		gfloat ratio = (float)old_height / (float)old_width;
		gfloat new_height = (width - 300) * ratio;
		if ((width - 300) != old_width) {
			if (new_height < height - 200) {
				gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(data->ui.image_pixbuf, width - 300, (width - 300) * ratio, GDK_INTERP_BILINEAR));
			}
		}
	}
	return FALSE;
}

int main () {
	gtk_init(NULL, NULL);

	// Loads the UI description and builds the UI
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "./src/ui/ui.glade", &error) == 0) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	// Gets the widgets
	// ====== Main ======
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
	GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_builder_get_object(builder, "header_bar"));
	GtkImage* main_image = GTK_IMAGE(gtk_builder_get_object(builder, "main_image"));
	GtkLabel* current_nn_main_path_label = GTK_LABEL(gtk_builder_get_object(builder, "current_nn_main_path_label"));
	// ====== Header Bar ======
	GtkButton* quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
	GtkButton* open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	GtkButton* resolve_button = GTK_BUTTON(gtk_builder_get_object(builder, "resolve_button"));
	// ======= Menu ======
	GtkModelButton* nn_menu_button = GTK_MODEL_BUTTON(gtk_builder_get_object(builder, "neural_network_menu_button"));
	GtkDialog* nn_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "neural_network_dialog"));
	GtkModelButton* about_menu_button = GTK_MODEL_BUTTON(gtk_builder_get_object(builder, "about_menu_button"));
	GtkDialog* about_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "about_dialog"));
	// ======= Neural Network ======
	GtkLabel* current_nn_path_label = GTK_LABEL(gtk_builder_get_object(builder, "current_nn_path_label"));
	GtkButton* nn_load_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_load_btn"));
	GtkButton* nn_generate_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_generate_btn"));
	GtkButton* nn_export_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_export_btn"));
	GtkInfoBar* nn_info_bar = GTK_INFO_BAR(gtk_builder_get_object(builder, "nn_info_bar"));
	GtkLabel* nn_info_bar_label = GTK_LABEL(gtk_builder_get_object(builder, "nn_info_bar_label"));
	GtkFileChooserButton* nn_file_chooser_btn = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "nn_file_chooser_btn"));
	// ====== Training NN =======
	GtkButton* nn_train_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_train_btn"));
	GtkFileChooserButton* train_file_chooser_btn = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "train_file_chooser_btn"));
	GtkSpinButton* epochs_spin_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "epochs_spin_btn"));
	GtkSpinButton* lr_spin_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "lr_spin_btn"));
	GtkTextView* train_logs_text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "train_logs_text_view"));

	// Close Info bar
	gtk_widget_hide(GTK_WIDGET(nn_info_bar));

	// Generate network
	size_t N_NEURONS_ARRAY[] = {N_NEURONS_INPUT, N_NEURONS_HIDDEN_1, N_NEURONS_OUTPUT};
	size_t sizes_inputs[] = {784, 784, 32};

	Layer network[N_LAYERS];
	generate_network(network, N_LAYERS, N_NEURONS_ARRAY, sizes_inputs);

	// Init ocr
	const char *default_network_path = "networks/default_network.net";
	init_ocr(default_network_path, network, current_nn_main_path_label, current_nn_path_label);

	// Create "ProgramData" structure
	ProgramData data = {
		.ui = {
			.window = window,
			.header_bar = header_bar,
			.main_image = main_image,
			.current_nn_main_path_label = current_nn_main_path_label,
			.resolve_button = resolve_button,
			.nn_dialog = nn_dialog,
			.image_pixbuf = NULL,
			.current_nn_path_label = current_nn_path_label,
			.nn_load_btn = nn_load_btn,
			.nn_info_bar = nn_info_bar,
			.nn_info_bar_label = nn_info_bar_label,
			.nn_file_chooser_btn = nn_file_chooser_btn,
			.nn_train_btn = nn_train_btn,
		},
		.net = {
			.network = network,
			.network_to_load_path = NULL,
			.layers = N_LAYERS,
			.sizes_neurons = N_NEURONS_ARRAY,
			.sizes_inputs = sizes_inputs,
			.epochs = DEFAULT_EPOCHS,
			.learning_rate = DEFAULT_LR,
			.dataset_path = NULL,
			.is_training = 0,
			.training_retval = -1,
		}
	};


	// Connects signal handlers
	g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &data);
	g_signal_connect_swapped(nn_menu_button, "clicked", G_CALLBACK(gtk_widget_show), nn_dialog);
	g_signal_connect_swapped(about_menu_button, "clicked", G_CALLBACK(gtk_widget_show), about_dialog);
	g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(on_configure_window), &data);
	g_signal_connect_swapped (about_dialog, "response", G_CALLBACK(gtk_widget_hide), about_dialog);
	g_signal_connect_swapped(about_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), about_dialog);
	// ====== Neural Network ======
	g_signal_connect(nn_generate_btn, "clicked", G_CALLBACK(on_nn_generate_btn), &data);
	g_signal_connect(nn_load_btn, "clicked", G_CALLBACK(on_nn_load_btn), &data);
	g_signal_connect(nn_export_btn, "clicked", G_CALLBACK(on_nn_export_btn), &data);
	g_signal_connect(nn_info_bar, "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(nn_file_chooser_btn, "file-set", G_CALLBACK(on_nn_load_file_set), &data);
	g_signal_connect(train_file_chooser_btn, "file-set", G_CALLBACK(on_train_file_set), &data);
	g_signal_connect(nn_train_btn, "clicked", G_CALLBACK(on_nn_train_btn), &data);
	g_signal_connect_swapped(nn_dialog, "response", G_CALLBACK(gtk_widget_hide), nn_dialog);
	g_signal_connect_swapped(nn_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), nn_dialog);

	gtk_window_set_title(GTK_WINDOW(window), "Sudoku Solver");

	// Runs the main loop
	gtk_main();
	free_network(network, data.net.layers);

	return 0;
}
