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

# Diary
A small note is that any day title with an asterisk is marked as a day I will have to revisit and refactor, because I was likely unhappy with the solution or structure.

## Day 01

There's not too much to say about day one. I can definitely feel that I would have solved this faster using trusty old C#. I had to learn the hard way that `strtok` mutates the data you're tokenizing by placing a null character where the delimiter is found, but once I got past that debacle I had no real hurdles.

## Day 02

Quite a simple day, I think this one took about five minutes, funnily enough this one went a lot quicker than yesterday's problem.

## Day 03 *
Ok, bare with me. The solution I'm currently comitting isn't exactly what I hoped it would be. I definitely know that my C# solution would have been a lot fancier, but I want to see that as me being in need of practice.

I was tempted to write the main function as `Part2(Part1())` and skip the whole `u32 length = Part1()` but I couldn't really allow myself. ;)

## Day 04
I'm already learning a thing or two about how I approach these problems compared to the usual aim. I typically want a dynamic easily modifiable solution that could fit almost any new change to the in-format. Which isn't as important during a test where you're solving a specific problem. I would have to be ready for the right-justification of numbers on the boards to change, the delimiters, the size of the boards, the amount of new lines between boards in the file and many other variables. I have, on purpose, chosen to not respect these possible changes in order to make something that solves this specific task. Thus, you will see things like `u32 boardSizeInFile = 16 /* columns in bytes */ * 5 /* rows */ + 2 /* \r\n */;`, which makes it easier to traverse to the next board, that spacing doesn't vary in the input (by design).

The second part of today's task was pretty quick, I copied the Part1 code and just made it able to remember the last winning one. I also had to add a boolean to check if the board had already won. I was happy that I didn't have to change too much, because this one took quite a while to do. Is this code perfect? Probably far from it, but I don't feel the need to revisit this one for refactoring.

## Day 05 *
Oh, there's something to unpack here. I know how this looks. It looks bad. This is all pretty slow, and the culprit is extremely likely that I check through every key in the hack I call `dictionary`. This could ofcourse be sped up immensly by making the key lookup more efficient. I don't use any libraries, so I can't just "cheat" my way here, so what I figure I'll do is to revisit this one and rewrite it to use some more reasonable way of finding keys quicker. I already know how, but I didn't want to put the effort in at the time of comitting this to git.