## Log Parser (2011)

```
Write a script that reads in a file specified on the command line. E.g
“./aggregate <filename>”

The file has the following format
<function>:<comma-separated integer list>

Example input
min:1,4,3,5,4,2,4,6
max:1,2,3,5,6,4,26,1,67
avg:7,2,2,8,6,3
p90:61,2,56,5,21


Where
Max=maximum
Min=minum
Avg=average
P90=The 90th percentile of the data.

The script will read in the file and produce line-by-line output by
applying each function to the corresponding list of integers and writing
the result to standard output followed by a newline.

Assume that this is an important piece of software that will be used by
many in a production environment.
```

## Reflection

This is an old sample code I wrote in 2011. Yes, it is horrible. I was
trying to refresh my C++ knoweldge after two years of pure PHP (2009).
At that time I did not write C++ for a while but it was intereseting to
see how I was writing C++. Now I did not write C++ since 2012. I wonder
how my code will look like. Another reflection in
[expression-tree](https://github.com/laithshadeed/expression-tree).

Looking at all this code right now, it makes me appreciate: *good code is the
one that makes money and you can easily modify (after years, months or days,
in this order)*. If it is easy to modify then it is easy to understand,
then it is easy to read, then it is beatiful code. If it does not make
money, but it makes you happy, then thats ok also :smile:

## Run & Compile

```
$ make
$ ./generate data.txt 1000 5000
$ ./aggregate data.txt
```

## LICENSE

Public Domain
