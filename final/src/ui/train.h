#include <gtk/gtk.h>

gpointer launch_training(gpointer user_data);

gboolean update_ui_when_training(gpointer user_data);

gboolean on_nn_train_btn(GtkWidget* widget, gpointer user_data);
