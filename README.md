# ZombieDash

A 2D run-and-gun video game, written in C++ and using FreeGLUT.

## Foreword/Forewarning

I fully realize that the whole point of making a video game is, indeed, to play it – however, my professor for this class wrote the actual *game* part of the game using [freeglut](http://freeglut.sourceforge.net/), and I was never actually able to get the game running using CLion and its bundled CMake; I actually had to copy over all the code I wrote in CLion over to XCode, from where the game would run fine upon hitting "run". I will investigate more into making the game run with CMake when I have the time, but for now running the project from XCode or compiling it manually should be fine.

## Introduction

This is my git repository to host my implementation of ZombieDash, the project assigned to us as Project 3 in [CS 32, Winter Quarter 2019](https://github.com/mppombo5/CS32) taught by Carey Nachenberg.

The code you see here is essentially the exact same code that I submitted for a grade. The only difference is that I've refactored the code to have better structure, and have written additional documentation that was not included in the final submission.

This project took me 60 hours out of two weeks to create, from the initial assignment to the due date. I received a 98 out of 100, only losing 2 points due to faulty behavior of Zombie vomit and vaccines.

## What is ZombieDash?

The spec for Project 3 of Winter 2019 is no longer accessible (and was roughly 65 pages long), but the long and short of it was to create a video game where you play as the hero, Penelope, using flamethrowers and landmines to save citizens from a room full of zombies. You, as the player, have to lead all the citizens in the level to the level's exit, all while avoiding the zombies that will shoot projectile vomit at you and infect you and other citizens if you let them get too close to you.

There are too many aspects of the game to fully recreate the specification, but I will do my best to describe all its intricacies in the documentation of this repository.

## The Assignment

The actual assignment only consisted of submitting four files: Actor.h, Actor.cpp, StudentWorld.h, and StudentWorld.cpp. All the other files govern how the game actually runs – these four files populate the game with everything that makes it fun to play. These files implement the actual actors and sprites and characters in the game, as well as the manipulation of said actors and how the game deals with them.
