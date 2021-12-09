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

NOTE: To be clear, don't read these notes if you don't expect spoilers. Since you're here to maybe check code, I'm sure that's not your worry - but I figure I'd atleast be kind and warn you that I'm discussing how I might have come about the solution or similar.

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

## Day 05
When I made this the first time around I didn't bother making the lookups quicker. They are still not the quickest you can get, but it's several seconds faster now and I'll be happy with this. I don't think I'm revisiting this one, but if I am I'll be looking into making the lookup even faster with some algorithm. I also feel like I could skip the many mallocs we create with the linked values. We could simply throw them into a large allocated space instead. We'll see if I update. If you're interested in what I originally wrote on this readme.md on day 05 first time around check commit 5bc1d342e133 or before.

## Day 06
This one was a pretty straight forward one, I think I saw the need to just throw it all into a "dictionary" and just play with the numbers. A typical red herring where you're lead to believe that handling every fish as its own entity/object might be a good idea, but it's just numbers. If you do want every fish to have individual weight, is where some problems arise. You see this in games when you might have many of one type of item, but what if they have different health values or slightly different wear and tear, all of a sudden you can't just say you have X of these items - atleast as easily. In this case we weren't tested on that part, instead we were tested by part 2 that we had already chosen a scalable approach.

## Day 07
I solved this one first by just iterating through the more exponential cost the crabs demanded in part 2, and I figured I could cache the answers for performance. Eventually I got to know that there's actually a nice formula for this `n(n+1)/2` that I slotted in instead. I put it into the method that calculates cost in general with the mode to be constantCost or the false version that isn't constant. In general there was no real problems with performance in this one, the versions I wrote performed but if the list of numbers were longer and the numbers a lot higher, the first solution would probably have been sluggish. I kind of started with the whole *QuickSort* thing because I wanted to do some "binary tree" search kind of deal, but in the end I just used it to spot when the cost takes a turn to isolate when the point of "cheapest" value has been met.

## Day 08 *
This one was quite the nightmare to pull off with this C business I'm doing, not using any classes. I definitely feel like I'm missing a lot of the usual tools but atleast I'm kind of getting a feel for every time I need to borrow some memory and understand that quite often a lot of things happen under the hood. Which ofcourse is a good thing, something usually managing all the stuff I don't necessarily want to muck around with - if I'm in a hurry. This one took an hour or two because writing all the pointer to pointer stuff, understanding the problem, thinking of what strategy I want to use to sus out the digits and some naming conventions. Speaking of, that's a mess I should clean up. I'm putting one of my asterisks down, and I'll look into maybe cleaning this mess up a bit.

## Day 09
Nothing much to say, pretty easy and straight forward. Took about 20 minutes for the first part and 20 for the second part, mostly wrestling with C things that would as usual be faster for me to write in C#, but I'm pretty ok with this one. Upwards and onwards!

# Benchmarks (approximates)
I'm using clock() for the time, basically record the clock() timestamp difference and multiply by 1000 and divice by CLOCKS_PER_SEC (which is 1000 on my end), and this will result in 0 ms when it's faster than a millisecond so it will show up as 0. I use __rdtsc() from <windows.h> to count cycles passed. As far as I know this is not cycles that MY executable used, it's how many passed since the timestamps, this means it varies a lot because of how much work windows needs the CPU to do in the background. In practice it's more of a timestamp than a clock-efficiency measurement.

In this current commit you can clearly see that Day 05 needs work.

```
- Day 01 -
Result Part 1: 1559 (0 ms, 552562 cycles passed)
Result Part 2: 1600 (0 ms, 448154 cycles passed)

- Day 02 -
Result Part 1: 2272262 (0 ms, 402934 cycles passed)
Result Part 2: 2134882034 (0 ms, 301857 cycles passed)

- Day 03 -
Result Part 1: 12 (0 ms, 249292 cycles passed)
Result Part 2: 6085575 (0 ms, 1771262 cycles passed)

- Day 04 -
Result Part 1: 8442 (0 ms, 736729 cycles passed)
Result Part 2: 4590 (0 ms, 1310847 cycles passed)

- Day 05 -
Result Part 1: 6572 (39 ms, 115516746 cycles passed)
Result Part 2: 21466 (607 ms, 1761207072 cycles passed)

- Day 06 -
Result Part 1: 356190 (0 ms, 306676 cycles passed)
Result Part 2: 1617359101538 (0 ms, 158363 cycles passed)

- Day 07 -
Result Part 1: 345197 (0 ms, 914613 cycles passed)
Result Part 2: 96361606 (0 ms, 1229731 cycles passed)

- Day 08 -
Result Part 1: 288 (0 ms, 1051050 cycles passed)
Result Part 2: 940724 (1 ms, 3836910 cycles passed)

- Day 09 -
Result Part 1: 439 (0 ms, 1412260 cycles passed)
Result Part 2: 900900 (1 ms, 1701717 cycles passed)
```