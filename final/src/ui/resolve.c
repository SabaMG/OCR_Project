#include <unistd.h>
#include <gtk/gtk.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"
#include "../preprocess/gauss.h"
#include "../preprocess/sobel.h"
#include "../preprocess/tools.h"
#include "../preprocess/rotation.h"
#include "../network/ocr.h"
#include "../solver/solver.h"
#include "../result/draw_result.h"

#define GRID_SIZE 9

// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format) {
	Uint8 r, g, b;
	// Get r, g, b from the pixel.
	SDL_GetRGB(pixel_color, format, &r, &g, &b);
	// Determine average.
	Uint8 average = 255 - (0.3*r + 0.59*g + 0.11*b);
	// Set r, g, b to average.
	r = average;
	g = average;
	b = average;
	// Return new color.
	return SDL_MapRGB(format, r, g, b);
}

void surface_to_grayscale(SDL_Surface* surface) {
	// Get array of pixels.
	Uint32* pixels = surface->pixels;
	// Get len of the array.
	int len = surface->w * surface->h;
	// Get the format of the surface.
	SDL_PixelFormat* format = surface->format;
	// Lock the surface.
	if (SDL_LockSurface(surface) != 0)
		printf("error\n");
		//errx(EXIT_FAILURE, "%s", SDL_GetError());
	// Convert each pixel to gray.
	for (int i = 0; i < len; i++) {
		pixels[i] = pixel_to_grayscale(pixels[i], format);
	}
	// Unlock the surface.
	SDL_UnlockSurface(surface);
}
// Define structure for an element of the async queue contening what to update in ui
typedef struct {
	char* text;
	double fraction;
	int has_to_update_pg;
	int has_to_update_img;
	SDL_Surface* surface;
} ui_queue_elt;

// Handler error function
gpointer fail_resolution(char *message, gpointer user_data) {
	ProgramData *data = user_data;

	// Add red color
	g_print("\033[0;31m");
	g_print("fail_resolution(): %s\n", message);
	g_print("\033[0m");

	data->img.is_resolving = 0;
	// Set resolving returned value to 0
	data->img.resolving_retval = 0;
	// Exit the thread
	g_thread_exit(NULL);
	return NULL;
}

// Update ui live when resolving
// Executed in a gtk timeout
gboolean update_ui_when_resolving(gpointer user_data) {
	ProgramData *data = user_data;

	ui_queue_elt* elt =  g_async_queue_try_pop(data->ui.ui_queue);

	if (data->img.has_been_canceled) {
		g_print("update_ui_when_resolving(): Has been canceled, kill ui updater\n");
		while (elt != NULL) {
			elt = g_async_queue_try_pop(data->ui.ui_queue);
		}
		return FALSE;
	}

	if (data->img.is_resolving == 0 && elt == NULL) {
		g_print("update_ui_when_resolving(): Resolving has stopped and queue is empty, kill ui updater\n");
		// Reset pg fraction and text
		gtk_progress_bar_set_fraction(data->ui.main_progress_bar, 0.f);
		gtk_progress_bar_set_text(data->ui.main_progress_bar, "");
		// Hide pg
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		// Set sensitivity of resolve btn to false
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), FALSE);
		// Update resolve_btn label
		gtk_button_set_label(data->ui.resolve_button, "Resolve");
		// Set sensitivity of save btn to true
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.save_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(data->ui.resolve_button), FALSE);
		return FALSE;
	}

	if (elt != NULL) {
		// Update pg if needed
		if (elt->has_to_update_pg == 1) {
			g_print("update_ui_when_resolving(): Updating progress bar --> %s\n", elt->text);
			gtk_progress_bar_set_text(data->ui.main_progress_bar, elt->text);
			double last_fraction = gtk_progress_bar_get_fraction(data->ui.main_progress_bar);
			gtk_progress_bar_set_fraction(data->ui.main_progress_bar, last_fraction + elt->fraction);
		}

		// Update img if needed
		if (elt->has_to_update_img == 1) {
			g_print("update_ui_when_resolving(): Updating image\n");
			GdkPixbuf *pixbuf = gtk_image_new_from_sdl_surface(elt->surface);

			gint original_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
			gint original_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);

			// Rescale pixbuf
			//data->ui.image_pixbuf = pixbuf; 
			gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
		}
		free(elt); // TODO: ici on free l'element mais pas la surface qu'il contient s'il devait modifier l'image !!!
	};

	return TRUE;
}

// Main resolution procedure
// Executed in a specific thread --> no ui freezing
gpointer resolution(gpointer user_data) {
	ProgramData *data = user_data;
	g_print("resolution(): Start Resolution\n");

	// Check if image can be opened
	if (data->img.opened_image_path == NULL)
		return fail_resolution("Failed to find opened image path", user_data);

	// ========================================================================
	// LOAD IMAGE
	// ========================================================================

	// Update pg
	ui_queue_elt* elt = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt->has_to_update_pg = 1;
	elt->text = "Loading Surface";
	elt->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt);

	SDL_Surface* original_surface = IMG_Load(data->img.opened_image_path);
	// Handle error
	if (original_surface == NULL)
		return fail_resolution("Failed to load image", user_data);

	// ========================================================================
	// CONVERT SURFACE FORMAT | SAVE ORIGINAL SURFACE
	// ========================================================================

	// Update pg
	ui_queue_elt* elt2 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt2->has_to_update_pg = 1;
	elt2->text = "Converting Surface Format";
	elt2->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt2);

	SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(original_surface, SDL_PIXELFORMAT_ARGB8888, 0);

	// Free old surface
	if (original_surface != NULL)
		SDL_FreeSurface(original_surface);

	// Handle error
	if (converted_surface == NULL)
		return fail_resolution("Failed to convert image", user_data);

	// ========================================================================
	// GAUSS FILTER
	// ========================================================================

	// Update pg
	ui_queue_elt* gauss_e = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	gauss_e->has_to_update_pg = 1;
	gauss_e->text = "Gauss Filter";
	gauss_e->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, gauss_e);

	SDL_Surface* gauss_surface = SDL_ConvertSurfaceFormat(converted_surface, SDL_PIXELFORMAT_ARGB8888, 0);
	gauss_surface = blur(gauss_surface, 3); // TODO: change param 3

	ui_queue_elt* gauss_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	gauss_i->has_to_update_img = 1;
	gauss_i->surface = gauss_surface;
	g_async_queue_push(data->ui.ui_queue, gauss_i);

	// ========================================================================
	// SOBEL FILTER
	// ========================================================================

	// Update pg
	ui_queue_elt* sobel_e = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	sobel_e->has_to_update_pg = 1;
	sobel_e->text = "Sobel Filter";
	sobel_e->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, sobel_e);

	//SDL_Surface* sobel_surface = sobel(gauss_surface);
	SDL_Surface* sobel_surface = sobel(converted_surface);

	// Update image
	ui_queue_elt* sobel_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	sobel_i->has_to_update_img = 1;
	sobel_i->surface = sobel_surface;
	g_async_queue_push(data->ui.ui_queue, sobel_i);

	// ========================================================================
	// IMAGE EDGES DETECTION | HOUGH
	// ========================================================================

	// Update pg
	ui_queue_elt* elt4 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt4->has_to_update_pg = 1;
	elt4->text = "Image Edges Detection";
	elt4->fraction = 0.1;

	g_async_queue_push(data->ui.ui_queue, elt4);

	int case_coor[81][2] = {};
	SDL_Surface boxesArray[81] = {};

	// Copy sobel_surface to segmentation_surface
	SDL_Surface* segmentation_surface = copy_surface(sobel_surface);

	// sobel_surface becomes segmentation surface;
	//int angle = segmentation(converted_surface, segmentation_surface, "", case_coor, boxesArray);

	// While angle is not equal to 0, rotate segmentation_surface
	int angle = segmentation(converted_surface, segmentation_surface, "", case_coor, boxesArray);
	g_print("bug\n");

	// Update image or do rotation
	if (angle == 0) {
		// Update image
		ui_queue_elt* seg_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
		seg_i->has_to_update_img = 1;
		seg_i->surface = segmentation_surface;
		g_async_queue_push(data->ui.ui_queue, seg_i);
	}
	else {

		SDL_Surface* rotated_surface = SDL_ConvertSurfaceFormat(segmentation_surface, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_Surface* tmp_segmentation_surface;

		while (angle != 0) {
			// ========================================================================
			// ROTATION AUTO
			// ========================================================================

			// Update pg
			ui_queue_elt* elt5 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			elt5->has_to_update_pg = 1;
			elt5->text = "Automatic Rotation";
			elt5->fraction = 0.1;
			g_async_queue_push(data->ui.ui_queue, elt5);

			rotated_surface = SDL_RotationCentral(rotated_surface, (float)angle);

			// Update image
			ui_queue_elt* rot_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			rot_i->has_to_update_img = 1;
			rot_i->surface = rotated_surface;
			g_async_queue_push(data->ui.ui_queue, rot_i);

			tmp_segmentation_surface = SDL_ConvertSurfaceFormat(rotated_surface, SDL_PIXELFORMAT_ARGB8888, 0);

			// SEGMENTATION
			// Update pg
			ui_queue_elt* seg_pg = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			seg_pg->has_to_update_pg = 1;
			seg_pg->text = "Image Edges Detection";
			seg_pg->fraction = 0.1;

			// Try again segmentation
			angle = segmentation(converted_surface, tmp_segmentation_surface, "", case_coor, boxesArray);
			g_print("resolution(): Angle after rotation = %i\n", angle);

			// Update image
			ui_queue_elt* seg_im = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			seg_im->has_to_update_img = 1;
			seg_im->surface = tmp_segmentation_surface;
			g_async_queue_push(data->ui.ui_queue, seg_im);
		}
		// Free old segmentation_surface and set it to tmp_surface
		if (segmentation_surface != NULL)
			SDL_FreeSurface(segmentation_surface);
		segmentation_surface = tmp_segmentation_surface;
	}


	// ========================================================================
	// RESIZE DIGIT IMAGE
	// ========================================================================

	// Update pg
	ui_queue_elt* elt7 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt7->has_to_update_pg = 1;
	elt7->text = "Resize Digit Image";
	elt7->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt7);

	/*
	   for (size_t i = 0; i < GRID_SIZE; i++) {
	   for (size_t j = 0; j < GRID_SIZE; j++) {
	   SDL_Surface digit_surface = boxesArray[GRID_SIZE * i + j];
	   g_print("w: %i, h: %i", digit_surface.w, digit_surface.h);
	//int res = ocr(data->net.network, digit_surface);
	//g_print("res = %i\n", res);
	//not_resolved_digits_grid[i][j] = res;
	}
	}
	*/

	// DEBUG: SAVE BOXES
	//Sets the directory to put the boxes into
	struct stat st;
	if (stat("./boxes", &st) == 0)
		printf("segmentation(): /boxes is present\n");
	else{
		if (mkdir("./boxes", 0700) == -1)
			g_print("ERROR: Unable to create ./boxes");
		printf("segmentation(): /boxes has been created\n");
	}

	char filename_[] = {'b', 'o', 'x', 'e', 's', '/', 'b', 'o', 'x', '_',
		'0', '0', '.', 'p', 'n', 'g', 0};

	for (size_t i = 0; i < 9; i++) {
		for (size_t j = 0; j < 9; j++) {
			SDL_Surface surf = boxesArray[GRID_SIZE * i + j];
			surface_to_grayscale(&surf);
			/*
			filename_[10] = '0' + i;
			filename_[11] = '0' + j;
			if (SDL_SaveBMP(&surf, filename_) == -1)
				printf("Unable to save the picture\n");
				*/
		}
	}


	// ========================================================================
	// OCR
	// ========================================================================

	// Update pg
	ui_queue_elt* elt8 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt8->has_to_update_pg = 1;
	elt8->text = "Digit Optical Recognition";
	elt8->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt8);

	int not_resolved_digits_grid[GRID_SIZE][GRID_SIZE] = {};

	for (size_t i = 0; i < GRID_SIZE; i++) {
		for (size_t j = 0; j < GRID_SIZE; j++) {
			SDL_Surface digit_surface = boxesArray[GRID_SIZE * i + j];
			SDL_Surface* surf =  SDL_ConvertSurfaceFormat(&digit_surface, SDL_PIXELFORMAT_ARGB8888, 0);
			SDL_Surface* n_surf = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0,0,0,0);

			filename_[10] = '0' + i;
			filename_[11] = '0' + j;

			char* res_s = calloc(2, sizeof(char));
			int res = ocr(data->net.network, surf, &n_surf);
			if (SDL_SaveBMP(n_surf, filename_) == -1)
				printf("Unable to save the picture\n");
			res_s[0] = '0' + res;
			//g_print("res = %s\n", res_s);
			not_resolved_digits_grid[j][i] = res;
			ui_queue_elt* q = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			q->has_to_update_pg = 1;
			q->text = res_s;
			q->has_to_update_img = 1;
			q->surface = surf;
			g_async_queue_push(data->ui.ui_queue, q);

			/*
			ui_queue_elt* q1 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
			q1->has_to_update_pg = 1;
			q1->text = "surface to feed";
			q1->has_to_update_img = 1;
			q1->surface = n_surf;
			g_async_queue_push(data->ui.ui_queue, q1);
			*/
		}
	}

	int image_grid[GRID_SIZE][GRID_SIZE] = {
	   {5,3,0,0,7,0,0,0,0},
	   {6,0,0,1,9,5,0,0,0},
	   {0,9,8,0,0,0,0,6,0},
	   {8,0,0,0,6,0,0,0,3},
	   {4,0,0,8,0,3,0,0,1},
	   {7,0,0,0,2,0,0,0,6},
	   {0,6,0,0,0,0,2,8,0},
	   {0,0,0,4,1,9,0,0,5},
	   {0,0,0,0,8,0,0,7,9},
	};	

	g_print("\n");
	for(size_t i = 0; i < GRID_SIZE; ++i) {
		for ( size_t j = 0; j < GRID_SIZE; ++j) {
			if(j % 3 == 0)
				printf(" ");
			if(not_resolved_digits_grid[i][j] != 0) {
				if (not_resolved_digits_grid[i][j] == image_grid[i][j]) {
					printf("\033[0;32m");
				}
				else {
					printf("\033[0;31m");
				}
				printf("[%i]", not_resolved_digits_grid[i][j]);
				printf("\033[0m");
			}
			else
				printf("[ ]");
		}
		if((i + 1) % 3 == 0)
			printf("\n");
		printf("\n");
	}

	g_print("resolution(): Finished resolution\n");

	data->img.is_resolving = 0;
	//data->img.resolving_retval = 1; // Stop normally
	g_thread_exit(NULL); //TODO: a enlever sinon bugg sur pie
	return NULL;
	//int not_resolved_digits_grid[GRID_SIZE][GRID_SIZE] = {};

	// TEST: solve and render on image 1
	/*
	   int not_resolved_digits_grid[GRID_SIZE][GRID_SIZE] = {
	   {5,3,0,0,7,0,0,0,0},
	   {6,0,0,1,9,5,0,0,0},
	   {0,9,8,0,0,0,0,6,0},
	   {8,0,0,0,6,0,0,0,3},
	   {4,0,0,8,0,3,0,0,1},
	   {7,0,0,0,2,0,0,0,6},
	   {0,6,0,0,0,0,2,8,0},
	   {0,0,0,4,1,9,0,0,5},
	   {0,0,0,0,8,0,0,7,9},
	   };	
	   */

	// All possible grids identified by the ocr
	//int all_possible_digits_grids[GRID_SIZE][GRID_SIZE][10] = {};
	int all_possible_digits_grids[GRID_SIZE][GRID_SIZE][10] = {
		{{5,-1},{3,-1},{0,-1},{0,-1},{7,-1},{0,-1},{0,-1},{0,-1},{0,-1}},
		{{8,6,-1},{0,-1},{0,-1},{1,-1},{9,-1},{5,-1},{0,-1},{0,-1},{0,-1}},
		{{0,-1},{9,-1},{8,-1},{0,-1},{0,-1},{0,-1},{0,-1},{6,-1},{0,-1}},
		{{8,-1},{0,-1},{0,-1},{0,-1},{6,-1},{0,-1},{0,-1},{0,-1},{3,-1}},
		{{4,-1},{0,-1},{0,-1},{8,-1},{0,-1},{3,-1},{0,-1},{0,-1},{1,-1}},
		{{7,-1},{0,-1},{0,-1},{0,-1},{2,-1},{0,-1},{0,-1},{0,-1},{6,-1}},
		{{0,-1},{6,-1},{0,-1},{0,-1},{0,-1},{0,-1},{2,-1},{8,-1},{0,-1}},
		{{0,-1},{0,-1},{0,-1},{4,-1},{1,-1},{9,-1},{0,-1},{0,-1},{5,-1}},
		{{0,-1},{0,-1},{0,-1},{0,-1},{8,-1},{0,-1},{0,-1},{7,-1},{9,-1}},
	};	


	// ========================================================================
	// SOLVE GRID
	// ========================================================================	

	// Update pg
	ui_queue_elt* elt9 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt9->has_to_update_pg = 1;
	elt9->text = "Sudoku Solving";
	elt9->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt9);

	int resolved_digits_grid[GRID_SIZE][GRID_SIZE] = {};

	int try_grid_indexes[GRID_SIZE][GRID_SIZE] = {};
	int current_try_digits_grid[GRID_SIZE][GRID_SIZE] = {};

	int is_another_possibility = 1;
	int k = 0;
	do {
		is_another_possibility = 0;
		// Build current_try_digits_grid
		for (size_t i = 0; i < GRID_SIZE; i++) {
			for (size_t j = 0; j < GRID_SIZE; j++) {
				k = try_grid_indexes[i][j];
				current_try_digits_grid[i][j] = all_possible_digits_grids[i][j][k];
				// Si il y a un autre choix possible
				if (all_possible_digits_grids[i][j][k + 1] != -1) {
					is_another_possibility = 1;
					try_grid_indexes[i][j]++;
				}
			}
		}
		//g_print("is a poss: %i\n", is_another_possibility);
	}
	while (solve(current_try_digits_grid, resolved_digits_grid) == 0 && is_another_possibility);

	// TEST: print resolved_digits_grid
	/*
	   g_print("\n");
	   for(size_t i = 0; i < GRID_SIZE; ++i) {
	   for ( size_t j = 0; j < GRID_SIZE; ++j) {
	   if(j % 3 == 0)
	   printf(" ");
	   if(resolved_digits_grid[i][j] != 0)
	   printf("[%i]", resolved_digits_grid[i][j]);
	   else
	   printf("[ ]");
	   }
	   if((i + 1) % 3 == 0)
	   printf("\n");
	   printf("\n");
	   }
	   */

	// ========================================================================
	// RENDER RESOLVED GRID
	// ========================================================================

	// Update pg
	ui_queue_elt* elt10 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt10->has_to_update_pg = 1;
	elt10->text = "Rendering Resolved Grid";
	elt10->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt10);


	// Load the empty grid and modify format
	SDL_Surface* init_grid = IMG_Load("./assets/empty_grid.jpg");
	if (init_grid == NULL)
		return fail_resolution("Failed to open 'empty_grid.jpg'", user_data);
	SDL_Surface* empty_grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
	if (empty_grid == NULL)
		return fail_resolution("Failed to convert init_grid surface", user_data);
	// Free init_grid surface
	SDL_FreeSurface(init_grid);

	// Save center coordinate of each boxes
	int coor[81][2] = {};
	collect_boxes_coor(empty_grid->w, empty_grid->h, coor, GRID_SIZE);

	// Draw on empty grid
	SDL_Surface* solved_sudo_surface = draw_empty(current_try_digits_grid, resolved_digits_grid);
	//SDL_Surface* solved_sudo_surface = draw_image(empty_grid, current_try_digits_grid, resolved_digits_grid, coor);

	if (solved_sudo_surface == NULL)
		return fail_resolution("Failed to draw result image", user_data);

	// Update image
	ui_queue_elt* elt11 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt10->has_to_update_pg = 1;
	elt10->text = "Rendering Resolved Grid";
	elt10->fraction = 0.1;
	elt11->has_to_update_img = 1;
	elt11->surface = solved_sudo_surface;
	g_async_queue_push(data->ui.ui_queue, elt11);

	//sleep(1);
	g_print("resolution(): Finished resolution\n");

	data->img.is_resolving = 0;
	//data->img.resolving_retval = 1; // Stop normally
	g_thread_exit(NULL); //TODO: a enlever sinon bugg sur pie
	return NULL;
}


// Callback of 'clicked' event on resolve button
gboolean on_resolve_button(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;

	if (data->img.is_resolving == 1 || g_async_queue_length(data->ui.ui_queue) != 0) {
		g_print("on_resolve_button(): Cancel Resolution\n");
		// Hide pg
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		// Set pg fraction to 0
		gtk_progress_bar_set_fraction(data->ui.main_progress_bar, 0.f);
		// Reset pg text
		gtk_progress_bar_set_text(data->ui.main_progress_bar, "");

		data->img.is_resolving = 0;
		data->img.has_been_canceled = 1;
		gtk_button_set_label(GTK_BUTTON(widget), "Resolve");
	}
	else {
		g_print("on_resolve_button(): Start Resolution\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Cancel");
		gtk_widget_show(GTK_WIDGET(data->ui.main_progress_bar));
		data->img.is_resolving = 1;
		data->img.has_been_canceled = 0;
		// Create new thread
		g_thread_new(NULL, resolution, user_data);
		// Create timeout for ui updates
		data->img.ui_update_timeout_id = g_timeout_add(1000, update_ui_when_resolving, user_data);
	}

	return FALSE;
}
