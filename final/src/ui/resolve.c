#include <unistd.h>
#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"
#include "../solver/solver.h"
#include "../result/draw_result.h"

#define GRID_SIZE 9

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
			data->ui.image_pixbuf = pixbuf; 
			gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
		}
		free(elt);
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

	// Update UI
	ui_queue_elt* elt = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt->has_to_update_pg = 1;
	elt->text = "Loading Surface";
	elt->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt);

	SDL_Surface* tmp_surf = IMG_Load(data->img.opened_image_path);
	// Handle error
	if (tmp_surf == NULL)
		return fail_resolution("Failed to load image", user_data);

	// ========================================================================
	// CONVERT SURFACE FORMAT | SAVE ORIGINAL SURFACE
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt2 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt2->has_to_update_pg = 1;
	elt2->text = "Converting Surface Format";
	elt2->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt2);

	//SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(tmp_surf, SDL_PIXELFORMAT_ARGB8888, 0);
	
	// Free old surface
	SDL_FreeSurface(tmp_surf);

	// Handle error
	if (tmp_surf == NULL)
		return fail_resolution("Failed to convert image", user_data);

	// ========================================================================
	// GAUSS FILTER
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt3 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt3->has_to_update_pg = 1;
	elt3->text = "Gauss Filter";
	elt3->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt3);




	// ========================================================================
	// IMAGE EDGES DETECTION
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt4 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt4->has_to_update_pg = 1;
	elt4->text = "Image Edges Detection";
	elt4->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt4);




	// ========================================================================
	// ROTATION AUTO
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt5 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt5->has_to_update_pg = 1;
	elt5->text = "Automatic Rotation";
	elt5->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt5);


	// ========================================================================
	// GRID EDGES DETECTION
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt6 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt6->has_to_update_pg = 1;
	elt6->text = "Grid Edges Detection";
	elt6->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt6);



	// ========================================================================
	// RESIZE DIGIT IMAGE
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt7 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt7->has_to_update_pg = 1;
	elt7->text = "Resize Digit Image";
	elt7->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt7);


	// ========================================================================
	// OCR
	// ========================================================================
	
	// Update UI
	ui_queue_elt* elt8 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
	elt8->has_to_update_pg = 1;
	elt8->text = "Digit Recognition";
	elt8->fraction = 0.1;
	g_async_queue_push(data->ui.ui_queue, elt8);

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
	
	// Update UI
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
	
	// Update UI
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

	// Update UI
	ui_queue_elt* elt11 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
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
		data->img.ui_update_timeout_id = g_timeout_add(300, update_ui_when_resolving, user_data);
	}

	return FALSE;
}
