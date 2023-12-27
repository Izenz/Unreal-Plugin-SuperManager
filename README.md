# Unreal Plugin: SuperManager
![Unreal_Banner](https://github.com/Izenz/unreal-custom-tools/assets/34793945/4b5cad29-fc22-4010-8f21-d2a8c47f3939)
## Overview
This repository contains an empty project to test different tools and functionality extensions for the **Unreal Engine 5 Editor**, contained inside a custom plugin called "Super Manager".<br />
Features inside the plugin are listed below.

Developed and tested using version 5.0.3. 

## Quick Asset Actions
Custom menu entries focusing on actions upon assets.
Right click an **Asset** in the **Content Browser** to access.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/6550b28c-67f2-46d5-a5c0-ef17053137e9">
</p>

:small_blue_diamond: **Add Prefixes**: Automatically adds <a href="https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/AssetNaming/">recommended prefixes</a> to selected assets. <br />
:small_blue_diamond: **Duplicate Assets**: Duplicates the selected assets, each for the specified amount of times. <br />
:small_blue_diamond: **Remove Unused Assets**: Prompts user to delete all unused assets that are also selected.<br />
:small_blue_diamond: **Rename Selected Assets**: Renames all assets, given a valid file name, and appends a number suffix.

## Quick Actor Actions
Custom menu entries focusing on actions upon assets.
Right click an **Actor** in the **Viewport Panel** to access.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/bae4ffd6-b511-41a2-b7f4-9951f6bcc977">
</p>

:small_blue_diamond: **Disable Actor Selection**: Prevents currently selected actor from being selected.<br />
:small_blue_diamond: **Enable All Actors Selection**: Lifts previous selection restriction from all assets in the current level.

## World Outliner Extension
<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/9cc35744-bcfe-4631-a20b-5e81393694e2">
</p>

Additional column displayed in the World Outliner to represent the Selection Lock state for each actor. It can be used as a Toggle Button.

## Content Browser Extensions
Functionality extensions to the content browser through custom context menu entries.
Right click any **Folder** in the **Content Browser** to access.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/a3ea6327-a532-4d83-8180-92b43a87fd81">
</p>

:small_blue_diamond: **Delete Unused Assets**: Deletes all assets not being used in current level inside selected folder.<br />
:small_blue_diamond: **Delete Empty Folders**: Search and delete all empty folders inside selected directory. <br />
:small_blue_diamond: **Open Advance Delete**: Invokes Advance Delete Slate Widget for the currently selected folder.

## Slate Widget
Custom Slate Widget providing some extra functionality for the user to delete assets.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/0e85a035-3521-481b-8793-99e620c8a40e">
</p>

:small_blue_diamond: Provides a list view for all the assets inside the selected folder. Listing asset type, name and allowing the user to mass select and delete them.

## Blueprint Tools
Blueprint tools providing some extra functionality upon selected actors and assets.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/e76e62b8-ac4d-43a8-ac23-cdcf2d772ed5">
</p>

:small_blue_diamond: Allows the user to select actors with a similar name to the currently selected actor. As well as provide features to batch duplicate actors and provide them with an offset and a rotation with the option to make them a random value.

<p align="center">
  <img src="https://github.com/Izenz/unreal-custom-tools/assets/34793945/59ce02d1-496c-4ec9-af90-d888c59177ef">
</p>

:small_blue_diamond: Allows user to automatically create a Material asset and it's Material Instance by selecting the Texture files and clicking panel button.
Provides modifiable fields to support custom naming conventions for assets in different projects.
