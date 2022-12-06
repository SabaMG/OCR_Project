#pragma once

//#include <gtk/gtk.h>
//#include "stdlib.h"
#include "../network/network.h"

// Structure of the graphical user interface.
typedef struct UserInterface {
    GtkWindow* window;
	GtkHeaderBar* header_bar;
	GtkImage* main_image;
	GtkLabel* current_nn_main_path_label;
	GtkButton* resolve_button;
	GtkDialog* nn_dialog;
	GdkPixbuf* image_pixbuf;
	GtkLabel* current_nn_path_label;
	GtkButton* nn_load_btn;
	GtkInfoBar* nn_info_bar;
	GtkLabel* nn_info_bar_label;
	GtkFileChooserButton* nn_file_chooser_btn;
	GtkButton* nn_train_btn;
} UserInterface;

typedef struct NetworkData {
	Layer* network;
	char *network_to_load_path;
	size_t layers;
	size_t *sizes_neurons;
	size_t *sizes_inputs;
	int epochs;
	float learning_rate;
	char *dataset_path;
	int is_training;
	int training_retval;
} NetworkData;

typedef struct ProgramData {
	UserInterface ui;
	NetworkData net;
} ProgramData;

