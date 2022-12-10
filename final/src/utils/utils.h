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
	GtkButton* save_button;
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
	GAsyncQueue* ui_queue;
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
	char *opened_image_path;
	int is_resolving;
	int has_been_canceled;
	int resolving_retval;
	int ui_update_timeout_id;
} ImageData;

typedef struct ProgramData {
	UserInterface ui;
	NetworkData net;
	ImageData img;
} ProgramData;

SDL_Surface *copy_surface(SDL_Surface *source);

GdkPixbuf * gtk_image_new_from_sdl_surface (SDL_Surface *surface);

Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format);

void surface_to_grayscale(SDL_Surface* surface);
