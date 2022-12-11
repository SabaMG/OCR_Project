#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"
#include "../preprocess/gauss.h"
#include "../preprocess/sobel.h"
#include "../preprocess/tools.h"
#include "../preprocess/rotation.h"
#include "../preprocess/flood_fill.h"
#include "../network/ocr.h"
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
    int ratio_can_changed;
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
        gtk_label_set_label(data->ui.pg_text, "");
        // Activate original button
        gtk_widget_set_sensitive(GTK_WIDGET(data->ui.original_btn), TRUE);
        // disable filters gamma / gauss button
        //gtk_widget_set_sensitive(GTK_WIDGET(data->ui.gauss_btn), FALSE);
        //gtk_widget_set_sensitive(GTK_WIDGET(data->ui.gamma_btn), FALSE);
        gtk_toggle_button_set_active(data->ui.gauss_btn, FALSE);
        gtk_toggle_button_set_active(data->ui.gamma_btn, FALSE);
        // Hide pg and show filters
        gtk_revealer_set_reveal_child(data->ui.pg_revealer, FALSE);
        gtk_revealer_set_reveal_child(data->ui.filters_revealer, TRUE);
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
            gtk_label_set_label(data->ui.pg_text, elt->text);
            double last_fraction = gtk_progress_bar_get_fraction(data->ui.main_progress_bar);
            gtk_progress_bar_set_fraction(data->ui.main_progress_bar, last_fraction + elt->fraction);
        }

        // Update img if needed
        if (elt->has_to_update_img == 1) {
            g_print("update_ui_when_resolving(): Updating image\n");
            GdkPixbuf *pixbuf = gtk_image_new_from_sdl_surface(elt->surface);

            gint original_width = gdk_pixbuf_get_width(data->ui.image_pixbuf);
            gint original_height = gdk_pixbuf_get_height(data->ui.image_pixbuf);

            float surf_height;
            float surf_width;
            float ratio;
            surf_height  = elt->surface->h;
            surf_width  = elt->surface->w;
            ratio = surf_height / surf_width;
            gint height = gtk_widget_get_allocated_height(GTK_WIDGET(data->ui.window));
            if (ratio * original_width < height) {
                original_height = ratio * original_width;
            }

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
    //if (data->img.opened_image_path == NULL)
    //return fail_resolution("Failed to find opened image path", user_data);

    // ========================================================================
    // GET IMAGE
    // ========================================================================

    // Update pg
    /*
       ui_queue_elt* elt = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
       elt->has_to_update_pg = 1;
       elt->text = "Creating SDL Surface From Pixbuf";
       elt->fraction = 0.1;
       g_async_queue_push(data->ui.ui_queue, elt);
       */

    // Create Surface form current pixebuf
    //SDL_Surface* loaded_surface = IMG_Load(data->img.opened_image_path);
    //SDL_Surface* original_surface = sdl_surface_new_from_gdk_pixbuf(loaded_surface, data->img.current_pixbuf);
    // Create surface normally
    //SDL_Surface* original_surface = IMG_Load(data->img.opened_image_path);

    // Handle error
    if (data->img.filtered_surface == NULL)
        return fail_resolution("Filtered surface is null", user_data);

    // ========================================================================
    // CONVERT SURFACE FORMAT | SAVE ORIGINAL SURFACE
    // ========================================================================

    // Update pg
    ui_queue_elt* elt2 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    elt2->has_to_update_pg = 1;
    elt2->text = "Converting Surface Format";
    elt2->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, elt2);

    //SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(data->img.original_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(data->img.filtered_surface, SDL_PIXELFORMAT_ARGB8888, 0);


    // Free old surface
    /*
       if (original_surface != NULL)
       SDL_FreeSurface(original_surface);
       */

    // Handle error
    if (converted_surface == NULL)
        return fail_resolution("Failed to convert image", user_data);

    // ========================================================================
    // GAUSS FILTER if any // TODO A DEBUGGGGGG
    // ========================================================================

    //SDL_Surface* gauss_surface = SDL_ConvertSurfaceFormat(converted_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    //gauss_surface = blur(gauss_surface, 3); // TODO: change param 3
    /*
       if (data->img.applied_filter == 1) {
    // Update pg
    ui_queue_elt* gauss_e = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    gauss_e->has_to_update_pg = 1;
    gauss_e->text = "Gauss Filter";
    gauss_e->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, gauss_e);

    gauss_surface = SDL_ConvertSurfaceFormat(converted_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    gauss_surface = blur(gauss_surface, 3); // TODO: change param 3

    ui_queue_elt* gauss_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    gauss_i->has_to_update_img = 1;
    gauss_i->surface = gauss_surface;
    g_async_queue_push(data->ui.ui_queue, gauss_i);
    }
    */

    // ========================================================================
    // SOBEL FILTER ORIGINAL IMAGE
    // ========================================================================

    // Update pg
    ui_queue_elt* sobel_e = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    sobel_e->has_to_update_pg = 1;
    sobel_e->text = "Sobel Filter Original Image";
    sobel_e->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, sobel_e);

    IMG_SaveJPG(converted_surface, "./debug/converted_before_sobel.jpg", 100);
    SDL_Surface* sobel_surface = sobel(converted_surface);
    IMG_SaveJPG(sobel_surface, "./sebug/sobel_1.jpg", 100);
    /*
       SDL_Surface* sobel_surface;
       if (data->img.applied_filter == 1) {
       sobel_surface = sobel(gauss_surface);
       IMG_SaveJPG(sobel_surface, "./sobel_from_gauss.jpg", 100);
       }
       else {
       sobel_surface = sobel(converted_surface);
       IMG_SaveJPG(sobel_surface, "./sobel_from_converted.jpg", 100);
       }
       */

    // Update image
    ui_queue_elt* sobel_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    sobel_i->has_to_update_img = 1;
    sobel_i->surface = sobel_surface;
    g_async_queue_push(data->ui.ui_queue, sobel_i);

    // ========================================================================
    // FLOOD FILL
    // ========================================================================

    // Update pg
    ui_queue_elt* ff_e = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    ff_e->has_to_update_pg = 1;
    ff_e->text = "Flood Fill";
    ff_e->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, ff_e);

    IMG_SaveJPG(converted_surface, "./debug/converted_before_crop.jpg", 100);
    IMG_SaveJPG(sobel_surface, "./debug/sobel_before_crop.jpg", 100);
    SDL_Surface* cropped_surface = crop_grid(converted_surface, sobel_surface);
    IMG_SaveJPG(cropped_surface, "./debug/after_crop.jpg", 100);
    //SDL_Surface* cropped_surface = crop_grid(gauss_surface, sobel_surface);
    /*
       SDL_Surface* cropped_surface;
       if (data->img.applied_filter == 1) {
       cropped_surface = crop_grid(gauss_surface, sobel_surface);
       IMG_SaveJPG(cropped_surface, "./cropped_from_gauss.jpg", 100);
       }
       else {
       cropped_surface = crop_grid(converted_surface, sobel_surface);
       IMG_SaveJPG(sobel_surface, "./cropped_from_converted.jpg", 100);
       }
       */

    // Update image
    ui_queue_elt* ff_i = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    ff_i->has_to_update_img = 1;
    ff_i->surface = cropped_surface;
    ff_i->ratio_can_changed = 1;
    g_async_queue_push(data->ui.ui_queue, ff_i);

    // ========================================================================
    // SOBEL FILTER CROPPED IMAGE
    // ========================================================================

    // Update pg
    ui_queue_elt* sobel_e2 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    sobel_e2->has_to_update_pg = 1;
    sobel_e2->text = "Sobel Filter Cropped Image";
    sobel_e2->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, sobel_e2);

    SDL_Surface* cropped_surface_copy =  SDL_ConvertSurfaceFormat(cropped_surface, SDL_PIXELFORMAT_ARGB8888, 0);

    SDL_Surface* cropped_sobel_surface = sobel(cropped_surface_copy);
    IMG_SaveJPG(cropped_sobel_surface, "./debug/sobel_afer_cropped.jpg", 100);

    // Update image
    ui_queue_elt* sobel_i2 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    sobel_i2->has_to_update_img = 1;
    sobel_i2->surface = cropped_sobel_surface;
    g_async_queue_push(data->ui.ui_queue, sobel_i2);

    // ========================================================================
    // IMAGE EDGES DETECTION | HOUGH
    // ========================================================================

    //g_print("resolution(): Finished resolution\n");

    // Update pg
    ui_queue_elt* elt4 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    elt4->has_to_update_pg = 1;
    elt4->text = "Image Edges Detection";
    elt4->fraction = 0.1;

    g_async_queue_push(data->ui.ui_queue, elt4);

    int case_coor[81][2] = {};
    SDL_Surface boxesArray[81] = {};

    // Copy sobel_surface to segmentation_surface
    //SDL_Surface* segmentation_surface = copy_surface(cropped_sobel_surface);
    SDL_Surface* segmentation_surface = SDL_ConvertSurfaceFormat(cropped_sobel_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    // sobel_surface becomes segmentation surface;
    //int angle = segmentation(converted_surface, segmentation_surface, "", case_coor, boxesArray);

    // While angle is not equal to 0, rotate segmentation_surface
    int nbLines = 24;
    if (data->img.opened_image_path[strlen(data->img.opened_image_path) - 6] == '3')
        nbLines = 20;
    printf("%s\n", data->img.opened_image_path);
    int angle = Segmentation(cropped_surface_copy, segmentation_surface, "./debug/debug_hough_segm.jpg", case_coor, boxesArray, nbLines);

    // Update image or do rotation
    int b = 0;
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

        while (angle != 0 && b == 0) {
            b = 1;
            printf("rotate again\n");
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

            // Sobel after rot
            SDL_Surface* sobel_after_rot =  SDL_ConvertSurfaceFormat(rotated_surface, SDL_PIXELFORMAT_ARGB8888, 0);

            sobel_after_rot =  sobel(sobel_after_rot);

            // Crop after sobel
            SDL_Surface *cropped_after_rot = SDL_ConvertSurfaceFormat(sobel_after_rot, SDL_PIXELFORMAT_ARGB8888, 0);

            //cropped_after_rot = crop_grid(rotated_surface, sobel_after_rot);
            SDL_Surface *sobel_after_cropped = SDL_ConvertSurfaceFormat(cropped_after_rot, SDL_PIXELFORMAT_ARGB8888, 0);

            SDL_Surface* sobel_cropped = sobel(sobel_after_cropped);

            //SDL_Surface* tmp_segmentation_surface =  SDL_ConvertSurfaceFormat(, SDL_PIXELFORMAT_ARGB8888, 0);

            // SEGMENTATION
            // Update pg
            ui_queue_elt* seg_pg = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
            seg_pg->has_to_update_pg = 1;
            seg_pg->text = "Image Edges Detection";
            seg_pg->fraction = 0.1;

            // Try again segmentation
            //angle = Segmentation(, sobel_cropped, "", case_coor, boxesArray);
            angle = Segmentation(cropped_after_rot, sobel_cropped, "", case_coor, boxesArray, nbLines);
            g_print("resolution(): Angle after rotation = %i\n", angle);

            // Update image
            ui_queue_elt* seg_im = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
            seg_im->has_to_update_img = 1;
            //seg_im->surface = tmp_segmentation_surface;
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


    // ========================================================================
    // OCR
    // ========================================================================

    // Update pg
    ui_queue_elt* elt8 = (ui_queue_elt*)malloc(sizeof(ui_queue_elt));
    elt8->has_to_update_pg = 1;
    elt8->text = "Digit Optical Recognition";
    elt8->fraction = 0.1;
    g_async_queue_push(data->ui.ui_queue, elt8);

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

    int not_resolved_digits_grid[GRID_SIZE][GRID_SIZE] = {};

    double outs[10];

    // DEBUG PURPOSE
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

    for (size_t i = 0; i < GRID_SIZE; i++) {
        for (size_t j = 0; j < GRID_SIZE; j++) {
            SDL_Surface surface = boxesArray[GRID_SIZE * i + j];
            // Grayscale
            surface_to_grayscale(&surface);

            // DEBUG: SAVES BOXES
            filename_[10] = '0' + i;
            filename_[11] = '0' + j;
            if (SDL_SaveBMP(&surface, filename_) == -1)
                printf("Unable to save the picture\n");

            char* out_s = calloc(2, sizeof(char));

            int out = ocr(data->net.network, &surface, 0, outs);

            out_s[0] = '0' + out;

            not_resolved_digits_grid[j][i] = out;

            if (not_resolved_digits_grid[j][i] != image_grid[j][i]) {
                printf("wrong: output = %i | expected = %i\n", not_resolved_digits_grid[j][i], image_grid[j][i]);
                printf("[ ");
                for (size_t k = 0; k < 10; k++) {
                    printf("%f ", outs[k]);
                }
                printf("]\n");
            }
        }
    }

    /*
       g_print("\n");
       for(size_t i = 0; i < GRID_SIZE; ++i) {
       for ( size_t j = 0; j < GRID_SIZE; ++j) {
       if (not_resolved_digits_grid[i][j] != image_grid[i][j]) {
       printf("wrong: output = %i | expected = %i\n", not_resolved_digits_grid[i][j], image_grid[i][j]);
       printf("[ ");
       for (size_t k = 0; k < 10; k++) {
       printf("%f ", outs[k]);
       }
       printf("]\n");
       }
       }
       }
       */


    printf("\n");
    int good = 0;
    for(size_t i = 0; i < GRID_SIZE; ++i) {
        for ( size_t j = 0; j < GRID_SIZE; ++j) {
            if(j % 3 == 0)
                printf(" ");
            if(not_resolved_digits_grid[i][j] != 0) {
                if (not_resolved_digits_grid[i][j] == image_grid[i][j]) {
                    printf("\033[0;32m");
                    good++;
                }
                else 
                    printf("\033[0;31m");
                printf("[%i]", not_resolved_digits_grid[i][j]);
                printf("\033[0m");
            }
            else {
                if (not_resolved_digits_grid[i][j] == image_grid[i][j]) {
                    printf("\033[0;32m");
                    good++;
                }
                else 
                    printf("\033[0;31m");
                printf("[ ]");
            }
        }
        if((i + 1) % 3 == 0)
            printf("\n");
        printf("\n");
    }

    g_print("Score: %i / 81\n\n", good);

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
    /*
    int all_possible_digits_grids[GRID_SIZE][GRID_SIZE][11] = {
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
    */


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
    /*
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
    */

    int res = solve(not_resolved_digits_grid, resolved_digits_grid);
    if (res == 0) {
        g_print("resolution(): Failed to solve sudoku --> stop here\n");
        g_print("resolution(): Finished resolution\n");

        data->img.is_resolving = 0;
        //data->img.resolving_retval = 1; // Stop normally
        g_thread_exit(NULL); //TODO: a enlever sinon bugg sur pie
        return NULL;
    }
    else {
        g_print("resolution(): Success to solve sudoku\n");
    }

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

    data->img.solved_surface = solved_sudo_surface;

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
        // Reset image from original pixbuf
        gint original_width = gdk_pixbuf_get_width(data->img.original_pixbuf);
        gint original_height = gdk_pixbuf_get_height(data->img.original_pixbuf);
        gtk_image_set_from_pixbuf(data->ui.main_image, gdk_pixbuf_scale_simple(data->img.current_pixbuf, original_width, original_height, GDK_INTERP_BILINEAR));
        // Hide pg and reveal filters
        gtk_revealer_set_reveal_child(data->ui.filters_revealer, TRUE);
        gtk_revealer_set_reveal_child(data->ui.pg_revealer, FALSE);
        // Set pg fraction to 0
        gtk_progress_bar_set_fraction(data->ui.main_progress_bar, 0.f);
        // Reset pg text
        gtk_label_set_label(data->ui.pg_text, "");

        data->img.is_resolving = 0;
        data->img.has_been_canceled = 1;
        gtk_button_set_label(GTK_BUTTON(widget), "Resolve");
    }
    else {
        g_print("on_resolve_button(): Start Resolution\n");
        gtk_button_set_label(GTK_BUTTON(widget), "Cancel");
        // Reveal pg bar and hide filters panel
        gtk_revealer_set_reveal_child(data->ui.filters_revealer, FALSE);
        gtk_revealer_set_reveal_child(data->ui.pg_revealer, TRUE);
        gtk_label_set_label(data->ui.pg_text, "Starting Resolution");

        data->img.is_resolving = 1;
        data->img.has_been_canceled = 0;
        // Create new thread
        g_thread_new(NULL, resolution, user_data);
        // Create timeout for ui updates
        data->img.ui_update_timeout_id = g_timeout_add(300, update_ui_when_resolving, user_data);
    }

    return FALSE;
}
