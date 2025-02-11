# Notelet 2.1 | the nonlinear notetaking app

Notelet is a notetaking app built upon stickynotes. it is my first experience in windows app development using wxWidgets in C++.

### Why wxWidgets?
Because I'm already familiar with c++ and wanted to learn something new without going into a completely different langauage. The framework is a bit limited though, especially on Windows, but it is fine :).

### What makes Notelet different than MS Notes & MS Whiteboard ?
Although these apps are developed by teams of great developers while I'm alone and still a beginner, the app offers some features that are not there (as far as I'm aware of). Those features include rich text formatting, to-do lists, and the most important thing I'm proud of: the app being developed by me alone in a short time frame. 

Watch the tutorial from [here](https://cloud-qnnkrogi9-hack-club-bot.vercel.app/02025-01-27_15-51-11.mp4)


## Features in Notelet 2.1
### Notes 
- Now resizable using the knob that apears down-right the note when activated by a click
- Supports rich text formatting. Enter text on the text panel down left, some shortcuts like:
    - **Ctrl + B** to make bold
    - **Ctrl + I** to make italic
    - **Ctrl + U** to make underlined
    - **Ctrl + L** to left align
    - **Ctrl + R** to right align
    - **Ctrl + E** to center align
    - **Ctrl + Shift + .** to increase font size
    - **Ctrl + Shift + ,** to decrease font size
    - Select any text + any of these shortcuts, and the formatting will be allplied to selection
- An activated note will show its content on the input box to edit text. Click update to apply your edit (edits include text or background color edit)
### Infinite number of colors
- Using the plus Icon on the color panel on the top-left, a dialog box will appear, choose your color format from HSL, RGB, or Hex, and then enter the color value
- Colors will show either their Hex code or RGB values when hovered on
### Images
- Import images from the tool bar or using **Ctrl + I**
### Imporved UI
- Only one side bar is being shown while other tools are moved to a tool bar at the top
### Multiple To-Do lists
- To do lists are now like notes: they can be resized, moved, and made multiple of
- To add a To-Do list, click of the to-do list button on the tool bar, **Ctrl + T** or go to Add>>To-Do list
- To rename a To-Do list, double click the title
### Dark/Light modes
- Go to settings >> switch Dark/Light modes
### Infinite space to work with
- Not really infinite, but you can Pan using right mouse button on the work space to navigate your notes giving more space to work with
### Drawing and erasing
- A brush tool is added to the tool bar, or use **Ctrl + B**
- An Eraser tool is also added to the tool bar, or use **Ctrl + E**
- When using either of them, new tools will appear on the right of the tool bar to change brush sizes
- 5 brushes are available: 1px, 2px, 3px, 5px, 10px all of them can be activated using **Ctrl + (1-5)**
- A brush takes the Foreground color from the side panel
- Shortcuts of brush and eraser will work only when the text box is not activated
- strokes will not overlay other components like notes and to-do lists (a limitation of wxwidgets)
- Custom cursors are addded for both tools
### Old features are still here
- You can select either a note or a to-do list and do the following:
  - **Delete** to delete
  - **Page Up** to raise above others
  - **Page down** to lower below others
- You can save, save as, and open files from the files menu
- The directory of file is now saved automatically, so you don't have to specify the path each time you save your work.
