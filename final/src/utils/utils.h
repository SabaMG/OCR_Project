#pragma once

//#include <gtk/gtk.h>
//#include "stdlib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
	GtkLabel* main_info_label;
	GtkProgressBar* main_progress_bar;
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

typedef struct ImageData {
	char *progress_bar_text;
	double progress_bar_fraction;
	char *opened_image_path;
	int is_resolving;
	int resolving_retval;
	int has_to_update_image;
	int has_to_update_progress_bar;
	SDL_Surface* original_surface;
	SDL_Surface* current_surface;
	GAsyncQueue* ui_queue;
} ImageData;

typedef struct ProgramData {
	UserInterface ui;
	NetworkData net;
	ImageData img;
} ProgramData;

SDL_Surface *copy_surface(SDL_Surface *source);

GdkPixbuf * gtk_image_new_from_sdl_surface (SDL_Surface *surface);
