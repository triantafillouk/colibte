# Color handling for ncurses driver

Colibte uses 10 background colors and 15 foreground colors. These can be defined for each scheme.

 In ncurses driver we must use color pairs.

# More than 16 colors

- This is easy, we suppose we have 256 or more pairs, 
 Some terminals (MS Windows terminal) use the first 16 colors for their own use, 
 so we define our colors after 17

# 8 colors
 We use color_index variable as a corresponding basic color. In any case we cannot have number of colors
 we really need


