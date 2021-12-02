# Advent of Code 2021 in C/C++

## Introduction
I'm doing this year's AoC in C/C++ which, despite my age, is fairly new to me. I primarily started with BASIC and moved onto C# when I was younger and never ventured into the land of pointers. There are many things I have yet to learn when it comes to how to structure and use the language fully to my own liking, but that's exactly what this exercise is about. I love doing AoC so why not also while learning something new.

I've been inspired by the likes of Casey Muratori (github.com/cmuratori) and his project Handmade Hero (a series where he teaches how to code a game from scratch using no libraries) is what started my adventure into C/C++ with a barebones kind of approach.

I usually try to do AoC on time, and at most I can be top 1000 and only ever once have I been top 100 on a single turn-in. This time however, I will focus on code and not time.

Also, you might notice I try to stick to older concepts, this is on purpose. I'm going to malloc, free and maintain a more procedural approach than an object oriented one.

## Compiling
I compile with **Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30038.1 for x64**.
```
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4100 -FC -Z7 -D_CRT_SECURE_NO_WARNINGS day01.cpp /link -opt:ref user32.lib
```

## Day 01

There's not too much to say about day one. I can definitely feel that I would have solved this faster using trusty old C#. I had to learn the hard way that `strtok` mutates the data you're tokenizing by placing a null character where the delimiter is found, but once I got past that debacle I had no real hurdles.

## Day 02

Quite a simple day, I think this one took about five minutes, funnily enough this one went a lot quicker than yesterday's problem.