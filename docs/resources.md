# Overview

Games often use media files made from specialized software (like Photoshop or Maya). These software export their file into commonly used format that  

 So we want to be able to load these media files into our game and read their information and use them. So for instance, we want to be able to load a png file, which contains a bitmap image, into a texture and display it as a sprite into our game.

# Technical requierments

# Solutions

## Directly import stuff

## Use extern tool

### Pros

* 
* 

### Cons

* 
* 




## Features

* We want to be able to keep track of the resources we already loaded and reuse them if needed, since we don't want to load the same resource twice since memory is a scarce resource
* We want a clear entry point in our engine to load any resource file from anywhere
* We want to be able to retrieve resources by their filepath relative to the resource folder
* We want to be able to convert the resources from various entry format, to a format the engine can understand
* We want to convert the resources from an outside tool because we want to keep our engine as simple and lightweight as possible

## Resource Identifier

A resource is simply identified by it's path relative to the resource folder. Since most operating systems guarantee that there's no 2 file with the same path/name.

## What is a resource

A resource is a often a media file that is needed by the game.

Since memory is a scarce resource, we want to avoid loading twice the same resource

Non exhaustive list of resources : 

* Pictures/Textures
* Music Files
* Mesh Files
* Material Files
* Tilemaps etc   

## Format and problems

But before going any further, we need to take into account the fact that a resource can exist into a wide variety of format. For instance, an image can be encoded TIFF, Bitmap, Jpeg, GIF, PNG etc.

This means that if we wanted to support all these formats and more, we would need to either directly write the importing code, or use libraries that would do it for us. 
In either case, it means more work for us poor dev, more dependencies and more overload for the actual engine running the game. 

So since we want to keep the actual engine as lightweight and simple as possible, what we're going to do is to create a tool that will scan the resource folder and convert every resource file it come accross into a unique format our engine can understand for each type of resource.

### Externalizing the conversion process

