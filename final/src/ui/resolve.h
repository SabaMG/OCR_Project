#include <gtk/gtk.h>

gpointer fail_resolution(char *message, gpointer user_data);

gboolean update_ui_when_resolving(gpointer user_data);

gpointer launch_resolution(gpointer user_data);

gboolean on_resolve_button(GtkWidget* widget, gpointer user_data);
