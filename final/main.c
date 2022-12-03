#include <gtk/gtk.h>

// Structure of the graphical user interface.
typedef struct UserInterface {
    GtkWindow* window;
	GtkHeaderBar* header_bar;
	GtkImage* main_image;
	GtkButton* resolve_button;
} UserInterface;

gboolean choose_file_dialog(GtkWidget *widget, gpointer user_data) {
	UserInterface *ui = user_data;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new (
			"Open File",
			ui->window,
			action,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT){
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);

		gtk_header_bar_set_subtitle(ui->header_bar, filename);
		GError *err = NULL;
		GdkPixbuf* image = gdk_pixbuf_new_from_file_at_scale (filename, 600, 600, TRUE, &err);
		if (err != NULL) {
			g_print("%s\n", err->message);
		}

		gtk_widget_set_opacity(GTK_WIDGET(ui->main_image), 1.f);
		gtk_image_set_from_pixbuf(ui->main_image, image);
		gtk_widget_set_sensitive(GTK_WIDGET(ui->resolve_button), TRUE);
	}

	gtk_widget_destroy (dialog);
	return FALSE;
}

int main (int argc, char **argv) {
	gtk_init(NULL, NULL);

	// Loads the UI description and builds the UI
	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "./ui.glade", &error) == 0) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	// Gets the widgets
	GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
	GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_builder_get_object(builder, "header_bar"));
	GtkImage* main_image = GTK_IMAGE(gtk_builder_get_object(builder, "main_image"));
	GtkButton* quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
	GtkButton* open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	GtkButton* resolve_button = GTK_BUTTON(gtk_builder_get_object(builder, "resolve_button"));

	// Create "UserInterface" structure
	UserInterface ui = {
		window,
		header_bar,
		main_image,
		resolve_button
	};

	// Connects signal handlers
	g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(choose_file_dialog), &ui);

	gtk_window_set_title(GTK_WINDOW(window), "Sudoku Solver");

	// Runs the main loop
	gtk_main();

	return 0;
}
