#include <unistd.h>
#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils/utils.h"

gpointer fail_resolution(char *message, gpointer user_data) {
	ProgramData *data = user_data;
	// Free Stuff
	if (data->img.original_surface != NULL) {
		SDL_FreeSurface(data->img.original_surface);
		SDL_FreeSurface(data->img.current_surface);
		data->img.original_surface = NULL;
		data->img.current_surface = NULL;
	}

	g_print("fail_resolution(): %s\n", message);
	data->img.is_resolving = 0;
	data->img.resolving_retval = 0; // error or cancel
	g_print("fail_resolution(): STOP PROCESS | EXIT THREAD\n");
	g_thread_exit(NULL);
	return NULL;
}

gboolean update_ui_when_resolving(gpointer user_data) {
	ProgramData *data = user_data;
	//g_print("Updating UI when resolving...\n");

	if (data->img.has_to_update_image == 1 && data->img.current_surface != NULL) {
		g_print("Updating main image\n");
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
	if (data->img.has_to_update_progress_bar == 1) {
		g_print("Updating progress barr\n");
		gtk_progress_bar_set_text(data->ui.main_progress_bar, data->img.progress_bar_text);
		gtk_progress_bar_set_fraction(data->ui.main_progress_bar, data->img.progress_bar_fraction);
		data->img.has_to_update_progress_bar = 0;
	}
	if (data->img.is_resolving == 0) {
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		gtk_button_set_label(data->ui.resolve_button, "Resolve");
		return FALSE;
	}
	return TRUE;
}

gpointer launch_resolution(gpointer user_data) {
	ProgramData *data = user_data;
	if (data->img.opened_image_path == NULL)
		return fail_resolution("fail to find opened image path", user_data);

	data->img.progress_bar_text = "Loading Surface";
	data->img.progress_bar_fraction += 0.3;
	data->img.has_to_update_progress_bar = 1;

	SDL_Surface* tmp_surf = IMG_Load(data->img.opened_image_path);
	if (tmp_surf == NULL)
		return fail_resolution("fail to load image", user_data);

	sleep(2);

	data->img.progress_bar_text = "Converting Surface Format";
	data->img.progress_bar_fraction += 0.3;
	data->img.has_to_update_progress_bar = 1;

	tmp_surf = SDL_ConvertSurfaceFormat(tmp_surf, SDL_PIXELFORMAT_ARGB8888, 0);
	if (tmp_surf == NULL)
		return fail_resolution("fail to convert image", user_data);

	data->img.original_surface = tmp_surf;
	// Copy original_surface to current_surface
	data->img.current_surface = copy_surface(data->img.original_surface);
	data->img.has_to_update_image = 1;
	g_print("Set has to update image to 1\n");

	sleep(2);

	data->img.progress_bar_text = "Gauss Filter";
	data->img.progress_bar_fraction += 0.3;
	data->img.has_to_update_progress_bar = 1;

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

	g_print("Finished resolution\n");

	// Free Stuff
	g_print("launch_resolution(): Start freeing...\n");
	if (data->img.original_surface != NULL)
		SDL_FreeSurface(data->img.original_surface);
	if (data->img.current_surface != NULL)
		SDL_FreeSurface(data->img.current_surface);

	g_print("launch_resolution(): Finish freeing...\n");
	data->img.original_surface = NULL;
	data->img.current_surface = NULL;

	data->img.is_resolving = 0;
	data->img.resolving_retval = 1; // Stop normally
	g_thread_exit(NULL);
	return NULL;
}

gboolean on_resolve_button(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	if (data->img.is_resolving) {
		g_print("Stop Resolving\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Resolve");
		gtk_widget_hide(GTK_WIDGET(data->ui.main_progress_bar));
		data->img.is_resolving = 0;
	}
	else {
		g_print("Start Resolving\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Cancel");
		gtk_widget_show(GTK_WIDGET(data->ui.main_progress_bar));
		data->img.is_resolving = 1;
		g_thread_new(NULL, launch_resolution, user_data);
		g_timeout_add(5, update_ui_when_resolving, user_data);
	}
	return FALSE;
}
