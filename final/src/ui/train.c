#include <gtk/gtk.h>

#include "../utils/utils.h"

#include "../network/ocr.h"
#include "../network/network.h"
#include "../network/training.h"

gpointer launch_training(gpointer user_data) {
	ProgramData *data = user_data;

	training(data->net.network, data->net.dataset_path, data->net.epochs, data->net.learning_rate, 1, user_data);

	//g_print("finished\n");
	data->net.is_training = 0;
	data->net.training_retval = 1; // Stop normally
	g_thread_exit(NULL);
	return NULL;
}

gboolean update_ui_when_training(gpointer user_data) {
	ProgramData *data = user_data;
	if (data->net.is_training == 0) {
		gtk_button_set_label(data->ui.nn_train_btn, "Launch Training");
		return FALSE;
	}
	return TRUE;
}

gboolean on_nn_train_btn(GtkWidget* widget, gpointer user_data) {
	ProgramData *data = user_data;
	if (data->net.is_training) {
		g_print("Stop Training\n");
		gtk_button_set_label(GTK_BUTTON(widget), "Launch Training");
		data->net.is_training = 0;
	}
	else {
		if (data->net.dataset_path == NULL) {
			gtk_info_bar_set_message_type(data->ui.nn_info_bar, GTK_MESSAGE_WARNING);
			gtk_widget_hide(GTK_WIDGET(data->ui.nn_info_bar));
			gtk_label_set_text(data->ui.nn_info_bar_label, "Please select a valid dataset.");
			gtk_widget_show(GTK_WIDGET(data->ui.nn_info_bar));
		}
		else {
			g_print("Start Training\n");
			gtk_button_set_label(GTK_BUTTON(widget), "Stop Training");
			data->net.is_training = 1;
			g_thread_new(NULL, launch_training, user_data);
			g_timeout_add(500, update_ui_when_training, user_data);
		}
	}
	return FALSE;
}

