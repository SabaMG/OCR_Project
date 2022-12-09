#include <unistd.h>
#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"
#include "../solver/solver.h"
#include "../result/draw_result.h"

#define GRID_SIZE 9

// Handler error function
gpointer fail_resolution(char *message, gpointer user_data) {
	ProgramData *data = user_data;

	// Free surfaces
	if (data->img.original_surface != NULL) {
		SDL_FreeSurface(data->img.original_surface);
		SDL_FreeSurface(data->img.current_surface);
		data->img.original_surface = NULL;
		data->img.current_surface = NULL;
	}
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

typedef struct
{
   int a;
} tint;


// Update ui live when resolving
// Executed in a gtk timeout
gboolean update_ui_when_resolving(gpointer user_data) {
	ProgramData *data = user_data;
	g_print("update_ui_when_resolving()\n");

	/*
	// If has to update ui image
	if (data->img.has_to_update_image == 1 && data->img.current_surface != NULL) {
		g_print("update_ui_when_resolving(): Updating main image\n");
		GdkPixbuf *pixbuf = gtk_image_new_from_sdl_surface(data->img.current_surface);

		// rescale pixbuf
		gint original_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
		gint original_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);
		gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));

		//gtk_image_set_from_pixbuf(data->ui.main_image, pixbuf);
		// TODO: free pixbuf ??
		data->img.has_to_update_image = 0;
		//g_print("Set has to update image to 0\n");
	}

	// If has to update progress bar
	if (data->img.has_to_update_progress_bar == 1) {
		g_print("update_ui_when_resolving(): Updating progress bar\n");
		gtk_progress_bar_set_text(data->ui.main_progress_bar, data->img.progress_bar_text);
		gtk_progress_bar_set_fraction(data->ui.main_progress_bar, data->img.progress_bar_fraction);
		data->img.has_to_update_progress_bar = 0;
	}

	// If resolution is finished or is canceled
	if (data->img.is_resolving == 0) {
		g_print("update_ui_when resolving(): Resolution finished or cancel, stop gtk timeout\n");
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		gtk_button_set_label(data->ui.resolve_button, "Resolve");
		// Stop gkt timeout by returning FALSE
		return FALSE;
	}
	*/
	gpointer e = g_async_queue_try_pop(data->img.ui_queue);
	if (data->img.is_resolving == 0 && e == NULL) {
		g_print("stop ui update\n");
		return FALSE;
	}

	tint* s = e;
	g_print("%i\n", s->a);
	free(s);

	return TRUE;
}

// Main resolution procedure
// Executed in a specific thread --> no ui freezing
gpointer resolution(gpointer user_data) {
	ProgramData *data = user_data;
	g_print("resolution(): Start Resolution\n");

	tint* ptr = (tint*)malloc(sizeof(tint));
	ptr->a = 5;
	g_async_queue_push(data->img.ui_queue, ptr);

	tint* ptr2 = (tint*)malloc(sizeof(tint));
	ptr2->a = 10;
	g_async_queue_push(data->img.ui_queue, ptr2);

	tint* ptr3 = (tint*)malloc(sizeof(tint));
	ptr3->a = 15;
	g_async_queue_push(data->img.ui_queue, ptr3);

	g_print("resolution(): Stop\n");
	data->img.is_resolving = 0;
	g_thread_exit(NULL);
	return NULL;

	data->img.resolving_retval = 0; // Cancel or error
									
	// Check if image can be opened
	if (data->img.opened_image_path == NULL)
		return fail_resolution("Failed to find opened image path", user_data);

	// ========================================================================
	// LOAD IMAGE
	// ========================================================================

	data->img.progress_bar_text = "Loading Surface";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;

	SDL_Surface* tmp_surf = IMG_Load(data->img.opened_image_path);
	// Handle error
	if (tmp_surf == NULL)
		return fail_resolution("Failed to load image", user_data);

	// ========================================================================
	// CONVERT SURFACE FORMAT | SAVE ORIGINAL SURFACE
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Converting Surface Format";
	data->img.progress_bar_fraction += 0.3;
	data->img.has_to_update_progress_bar = 1;

	tmp_surf = SDL_ConvertSurfaceFormat(tmp_surf, SDL_PIXELFORMAT_ARGB8888, 0);
	// Handle error
	if (tmp_surf == NULL)
		return fail_resolution("Failed to convert image", user_data);

	data->img.original_surface = tmp_surf;
	// Copy original_surface to current_surface
	data->img.current_surface = copy_surface(data->img.original_surface);
	// Update ui image
	data->img.has_to_update_image = 1;

	// ========================================================================
	// GAUSS FILTER
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Gauss Filter";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;

	// Update ui image

	



	// ========================================================================
	// IMAGE EDGES DETECTION
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Image Edges Detection";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;

	// Update ui image



	// ========================================================================
	// ROTATION AUTO
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Automatic Rotation";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;


	// ========================================================================
	// GRID EDGES DETECTION
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Grid Edges Detection";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;



	// ========================================================================
	// RESIZE DIGIT IMAGE
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Resize Digit Image";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;


	// ========================================================================
	// OCR
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Digit Recognition";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;

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
	// Update progress bar
	data->img.progress_bar_text = "Sudoku Solving";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;

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

	// ========================================================================
	// RENDER RESOLVED GRID
	// ========================================================================
	// Update progress bar
	data->img.progress_bar_text = "Rendering Resolved Grid";
	data->img.progress_bar_fraction += 0.1;
	data->img.has_to_update_progress_bar = 1;
	update_ui_when_resolving(user_data);

	// Load the empty grid and modify format
	SDL_Surface* init_grid = IMG_Load("./assets/empty_grid.jpg");
	if (init_grid == NULL)
		return fail_resolution("Failed to open 'empty_grid.jpg'", user_data);
	SDL_Surface* empty_grid = SDL_ConvertSurfaceFormat(init_grid, SDL_PIXELFORMAT_ARGB8888, 0);
	if (empty_grid == NULL)
		return fail_resolution("Failed to convert init_grid surface", user_data);
    SDL_FreeSurface(init_grid);

	// Save center coordinate of each boxes
    int coor[81][2] = {};
    collect_boxes_coor(empty_grid->w, empty_grid->h, coor, GRID_SIZE);

	// No need to free -> current_image_surface already freeing
    SDL_Surface* solved_sudo_surface = draw_image(empty_grid, current_try_digits_grid, resolved_digits_grid, coor);
	if (solved_sudo_surface == NULL)
		return fail_resolution("Failed to draw result image", user_data);

	// Free current_surface
	if (data->img.current_surface != NULL)
		SDL_FreeSurface(data->img.current_surface);
	// Set current_surface to solved_sudo_surface
	data->img.current_surface = solved_sudo_surface;
	// Update ui image
	g_print("has to u: %i\n", data->img.has_to_update_image);
	data->img.has_to_update_image = 1;
	g_print("has to u: %i\n", data->img.has_to_update_image);
	// TODO : Any better solution ?

	// Simulate computation
	/*
	   while(1) {
	   if (data->img.is_resolving == 0) {
	   data->img.is_resolving = 0;
	   data->img.resolving_retval = 0; // Stop before to finish
	   g_print("Cancel\n");
	   g_thread_exit(NULL);
	   }
	   }
	   */
	/*
	while (data->img.has_to_update_image != 0) {
		g_print("has to u: %i\n", data->img.has_to_update_image);
	}
	*/

	g_print("resolution(): Finished resolution\n");

	// Free Stuff
	g_print("resolution(): Start freeing surfaces\n");
	if (data->img.original_surface != NULL)
		SDL_FreeSurface(data->img.original_surface);

	if (data->img.current_surface != NULL)
		SDL_FreeSurface(data->img.current_surface);

	// !!!!! BUGGY !!!!!
	//if (init_grid != NULL)
		//SDL_FreeSurface(init_grid);
	//if (empty_grid != NULL)
		//SDL_FreeSurface(empty_grid);

	g_print("resolution(): Finish freeing surfaces\n");

	data->img.original_surface = NULL;
	data->img.current_surface = NULL;
	data->img.has_to_update_image = 0;
	data->img.has_to_update_progress_bar = 0;

	data->img.is_resolving = 0;
	data->img.resolving_retval = 1; // Stop normally
	g_thread_exit(NULL);
	return NULL;
}


// Callback of 'clicked' event on resolve button
gboolean on_resolve_button(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;

	if (data->img.is_resolving) {
		g_print("on_resolve_button(): Cancel Resolution\n");
		gtk_widget_set_sensitive(widget, FALSE);
		sleep(1);
		gtk_widget_set_sensitive(widget, TRUE);
		gtk_button_set_label(GTK_BUTTON(widget), "Resolve");
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		data->img.is_resolving = 0;
	}
	else {
		//g_print("on_resolve_button(): Start Resolution\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Cancel");
		gtk_widget_show(GTK_WIDGET(data->ui.main_progress_bar));
		data->img.is_resolving = 1;
		gtk_widget_set_sensitive(widget, FALSE);
		gtk_widget_set_sensitive(widget, TRUE);
		// Create new thread
		g_thread_new(NULL, resolution, user_data);
		// Create timeout for ui updates
		g_timeout_add(500, update_ui_when_resolving, user_data);
	}

	return FALSE;
}
