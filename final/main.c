#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "src/ui/ui.h"
#include "src/ui/train.h"
#include "src/ui/resolve.h"
#include "src/utils/utils.h"

#include "src/network/ocr.h"
#include "src/network/network.h"
#include "src/network/training.h"

#define N_LAYERS 3
#define N_NEURONS_INPUT 784
#define N_NEURONS_HIDDEN_1 32
#define N_NEURONS_OUTPUT 10

#define DEFAULT_EPOCHS 20
#define DEFAULT_LR 0.1

int main () {

	// Generate network
	size_t N_NEURONS_ARRAY[] = {N_NEURONS_INPUT, N_NEURONS_HIDDEN_1, N_NEURONS_OUTPUT};
	size_t sizes_inputs[] = {784, 784, 32};

	Layer network[N_LAYERS];
	generate_network(network, N_LAYERS, N_NEURONS_ARRAY, sizes_inputs);
	

	// Initialize GTK
	gtk_init(NULL, NULL);


	// Load ui from glade file
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "./assets/ui.glade", &error) == 0) {
		//g_printerr("init_ui(): Error loading file: %s\n", error->message);
		g_clear_error(&error);
		errx(EXIT_FAILURE, "%s", error->message);
	}


	// Get widgets
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
	GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_builder_get_object(builder, "header_bar"));
	GtkImage* main_image = GTK_IMAGE(gtk_builder_get_object(builder, "main_image"));
	GtkInfoBar* nn_info_bar = GTK_INFO_BAR(gtk_builder_get_object(builder, "nn_info_bar"));
	GtkProgressBar* main_progress_bar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "main_progress_bar"));

	GtkLabel* current_nn_main_path_label = GTK_LABEL(gtk_builder_get_object(builder, "current_nn_main_path_label"));
	GtkLabel* main_info_label = GTK_LABEL(gtk_builder_get_object(builder, "main_info_label"));
	GtkLabel* nn_info_bar_label = GTK_LABEL(gtk_builder_get_object(builder, "nn_info_bar_label"));
	GtkLabel* current_nn_path_label = GTK_LABEL(gtk_builder_get_object(builder, "current_nn_path_label"));
	GtkTextView* train_logs_text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "train_logs_text_view"));

	GtkButton* quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
	GtkButton* open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	GtkButton* resolve_button = GTK_BUTTON(gtk_builder_get_object(builder, "resolve_button"));
	GtkButton* nn_load_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_load_btn"));
	GtkButton* nn_generate_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_generate_btn"));
	GtkButton* nn_export_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_export_btn"));
	GtkButton* nn_train_btn = GTK_BUTTON(gtk_builder_get_object(builder, "nn_train_btn"));
	GtkSpinButton* epochs_spin_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "epochs_spin_btn"));
	GtkSpinButton* lr_spin_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "lr_spin_btn"));
	GtkModelButton* nn_menu_button = GTK_MODEL_BUTTON(gtk_builder_get_object(builder, "neural_network_menu_button"));
	GtkModelButton* about_menu_button = GTK_MODEL_BUTTON(gtk_builder_get_object(builder, "about_menu_button"));
	GtkFileChooserButton* nn_file_chooser_btn = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "nn_file_chooser_btn"));
	GtkFileChooserButton* train_file_chooser_btn = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "train_file_chooser_btn"));

	GtkDialog* nn_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "neural_network_dialog"));
	GtkDialog* about_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "about_dialog"));


	// Create ProgramData struct
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
			.main_info_label = main_info_label,
			.main_progress_bar = main_progress_bar,
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
		},
		.img = {
			.progress_bar_text = NULL,
			.progress_bar_fraction = 0.f,
			.opened_image_path = NULL,
			.is_resolving = 0,
			.resolving_retval = 0,
			.has_to_update_image = 0,
			.has_to_update_progress_bar = 0,
			.original_surface = NULL,
			.current_surface = NULL,
		},
	};

	// Set window title
	gtk_window_set_title(GTK_WINDOW(window), "Sudoku Solver");
	// Close info bar
	gtk_widget_hide(GTK_WIDGET(nn_info_bar));
	// Hide progress bar and set fraction to 0
	gtk_widget_hide(GTK_WIDGET(main_progress_bar));
	gtk_progress_bar_set_fraction(main_progress_bar, 0.f);


	// Connects signal handlers
	g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(on_configure_window), &data);
	g_signal_connect(nn_info_bar, "response", G_CALLBACK(gtk_widget_hide), NULL);
	g_signal_connect(resolve_button, "clicked", G_CALLBACK(on_resolve_button), &data);

	g_signal_connect_swapped(nn_menu_button, "clicked", G_CALLBACK(gtk_widget_show), nn_dialog);
	g_signal_connect_swapped(about_menu_button, "clicked", G_CALLBACK(gtk_widget_show), about_dialog);
	g_signal_connect_swapped (about_dialog, "response", G_CALLBACK(gtk_widget_hide), about_dialog);
	g_signal_connect_swapped(about_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), about_dialog);

	g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &data);
	g_signal_connect(nn_generate_btn, "clicked", G_CALLBACK(on_nn_generate_btn), &data);
	g_signal_connect(nn_load_btn, "clicked", G_CALLBACK(on_nn_load_btn), &data);
	g_signal_connect(nn_export_btn, "clicked", G_CALLBACK(on_nn_export_btn), &data);

	g_signal_connect(nn_train_btn, "clicked", G_CALLBACK(on_nn_train_btn), &data);
	g_signal_connect(lr_spin_btn, "value-changed", G_CALLBACK(on_lr_spin_btn), &data);
	g_signal_connect(epochs_spin_btn, "value-changed", G_CALLBACK(on_epochs_spin_btn), &data);

	g_signal_connect(nn_file_chooser_btn, "file-set", G_CALLBACK(on_nn_load_file_set), &data);
	g_signal_connect(train_file_chooser_btn, "file-set", G_CALLBACK(on_train_file_set), &data);

	g_signal_connect_swapped(nn_dialog, "response", G_CALLBACK(gtk_widget_hide), nn_dialog);
	g_signal_connect_swapped(nn_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), nn_dialog);


	// Initialize OCR
	const char *default_network_path = "networks/default_network.net";
	printf("%s\n", gtk_label_get_text(data.ui.current_nn_main_path_label));
	ocr_init(default_network_path, network, data.ui.current_nn_main_path_label, data.ui.current_nn_path_label);


	// Runs the main loop
	gtk_main();


	// Free network
	free_network(network, data.net.layers);
	// Free surfaces
	if (data.img.original_surface != NULL)
		SDL_FreeSurface(data.img.original_surface);
	if (data.img.current_surface != NULL)
		SDL_FreeSurface(data.img.current_surface);
	return 0;
}
