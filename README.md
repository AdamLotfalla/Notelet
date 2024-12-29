# Notelet - the nonlinear notetaking app

Notelet is a notetaking app built upon stickynotes. it is my first experience in windows app development using wxWidgets in C++.

The app currently offers only stickynotes with all of their functionalities but I aim to add other features like annotation and images 

## Test the app
Watch a demo video [here](https://drive.google.com/file/d/1TVw4F6kulGVK57ZKp9LZ-EN0cC3JiGdy/view?usp=drive_link)
, or download the program for this [drive](https://drive.google.com/drive/folders/1eHFIcFspGgeAo4dQFW-o4aptjiVGgcJl?usp=sharing)


https://github.com/user-attachments/assets/534c1b1f-5d58-412c-bccf-e045d98046bd


## Basic controls
- On the left panel, you have the color pane on the top. You can choose what color to change: foreground or background, and then choose the color for that choice. The color chosen will appear beside its corresponding radiobox.
- On the left panel, you have the add note options on the bottom. First enter your note text in the text box, choose your formatting: **bold**, **italic**, or **Underlined**, then click **add**, **Ctrl + N**, or go to **Add >> New note** menu to create new.
- If you want to modify the note, click on the desired note, select your modificaitons, enter the new text, and then click *update*.
- You can drag any note on the work area
- To put a note on the top, first select the note and then click **Home** key on the keyboard
- To put a note on the bottom, first select the note and then click **End** key on the keyboard
- To delete a note, select the desired note, and press **Delete** key on the keyboard
- On the right panel, you can add tasks to your To-Do list
- Enter the description of your task in the text box above, click add or press **Enter** key to add the task
- On the bottom you have controls
- To rearrange tasks, select the desired task to move and click either arrow on the bottm right
- To delete the selected task, click "Delete task"
- To clear the To-Do list, click on the "Clear" button, a dialog box will appear to confirm your command. Click "Yes" to delete all, or "No"/"Cancel" to go back
- To Save your work, go to **File >> Save As**
- To open an old workspace, go to **File >> Open** and select your .XML file
