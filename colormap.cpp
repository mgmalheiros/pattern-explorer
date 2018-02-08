/*-------------------------------- INCLUDES --------------------------------*/

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "colormap.hpp"

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static const char *names[280] = {
		"f0f8ff", "aliceblue",
		"faebd7", "antiquewhite",
		"00ffff", "aqua",
		"7fffd4", "aquamarine",
		"f0ffff", "azure",
		"f5f5dc", "beige",
		"ffe4c4", "bisque",
		"000000", "black",
		"ffebcd", "blanchedalmond",
		"0000ff", "blue",
		"8a2be2", "blueviolet",
		"a52a2a", "brown",
		"deb887", "burlywood",
		"5f9ea0", "cadetblue",
		"7fff00", "chartreuse",
		"d2691e", "chocolate",
		"ff7f50", "coral",
		"6495ed", "cornflowerblue",
		"fff8dc", "cornsilk",
		"dc143c", "crimson",
		"00ffff", "cyan",
		"00008b", "darkblue",
		"008b8b", "darkcyan",
		"b8860b", "darkgoldenrod",
		"a9a9a9", "darkgray",
		"006400", "darkgreen",
		"bdb76b", "darkkhaki",
		"8b008b", "darkmagenta",
		"556b2f", "darkolivegreen",
		"ff8c00", "darkorange",
		"9932cc", "darkorchid",
		"8b0000", "darkred",
		"e9967a", "darksalmon",
		"8fbc8f", "darkseagreen",
		"483d8b", "darkslateblue",
		"2f4f4f", "darkslategray",
		"00ced1", "darkturquoise",
		"9400d3", "darkviolet",
		"ff1493", "deeppink",
		"00bfff", "deepskyblue",
		"696969", "dimgray",
		"1e90ff", "dodgerblue",
		"b22222", "firebrick",
		"fffaf0", "floralwhite",
		"228b22", "forestgreen",
		"ff00ff", "fuchsia",
		"dcdcdc", "gainsboro",
		"f8f8ff", "ghostwhite",
		"ffd700", "gold",
		"daa520", "goldenrod",
		"808080", "gray",
		"008000", "green",
		"adff2f", "greenyellow",
		"f0fff0", "honeydew",
		"ff69b4", "hotpink",
		"cd5c5c", "indianred",
		"4b0082", "indigo",
		"fffff0", "ivory",
		"f0e68c", "khaki",
		"e6e6fa", "lavender",
		"fff0f5", "lavenderblush",
		"7cfc00", "lawngreen",
		"fffacd", "lemonchiffon",
		"add8e6", "lightblue",
		"f08080", "lightcoral",
		"e0ffff", "lightcyan",
		"fafad2", "lightgoldenrodyellow",
		"90ee90", "lightgreen",
		"d3d3d3", "lightgrey",
		"ffb6c1", "lightpink",
		"ffa07a", "lightsalmon",
		"20b2aa", "lightseagreen",
		"87cefa", "lightskyblue",
		"778899", "lightslategray",
		"b0c4de", "lightsteelblue",
		"ffffe0", "lightyellow",
		"00ff00", "lime",
		"32cd32", "limegreen",
		"faf0e6", "linen",
		"ff00ff", "magenta",
		"800000", "maroon",
		"66cdaa", "mediumauqamarine",
		"0000cd", "mediumblue",
		"ba55d3", "mediumorchid",
		"9370d8", "mediumpurple",
		"3cb371", "mediumseagreen",
		"7b68ee", "mediumslateblue",
		"00fa9a", "mediumspringgreen",
		"48d1cc", "mediumturquoise",
		"c71585", "mediumvioletred",
		"191970", "midnightblue",
		"f5fffa", "mintcream",
		"ffe4e1", "mistyrose",
		"ffe4b5", "moccasin",
		"ffdead", "navajowhite",
		"000080", "navy",
		"fdf5e6", "oldlace",
		"808000", "olive",
		"688e23", "olivedrab",
		"ffa500", "orange",
		"ff4500", "orangered",
		"da70d6", "orchid",
		"eee8aa", "palegoldenrod",
		"98fb98", "palegreen",
		"afeeee", "paleturquoise",
		"d87093", "palevioletred",
		"ffefd5", "papayawhip",
		"ffdab9", "peachpuff",
		"cd853f", "peru",
		"ffc0cb", "pink",
		"dda0dd", "plum",
		"b0e0e6", "powderblue",
		"800080", "purple",
		"ff0000", "red",
		"bc8f8f", "rosybrown",
		"4169e1", "royalblue",
		"8b4513", "saddlebrown",
		"fa8072", "salmon",
		"f4a460", "sandybrown",
		"2e8b57", "seagreen",
		"fff5ee", "seashell",
		"a0522d", "sienna",
		"c0c0c0", "silver",
		"87ceeb", "skyblue",
		"6a5acd", "slateblue",
		"708090", "slategray",
		"fffafa", "snow",
		"00ff7f", "springgreen",
		"4682b4", "steelblue",
		"d2b48c", "tan",
		"008080", "teal",
		"d8bfd8", "thistle",
		"ff6347", "tomato",
		"40e0d0", "turquoise",
		"ee82ee", "violet",
		"f5deb3", "wheat",
		"ffffff", "white",
		"f5f5f5", "whitesmoke",
		"ffff00", "yellow",
		"9acd32", "yellowgreen"
};

static float heat[300] = {
        0.000F, 0.000F, 1.000F,  0.000F, 0.043F, 1.000F,  0.000F, 0.082F, 1.000F,  0.000F, 0.122F, 1.000F,
        0.000F, 0.161F, 1.000F,  0.000F, 0.200F, 1.000F,  0.000F, 0.239F, 1.000F,  0.000F, 0.282F, 1.000F,
        0.000F, 0.322F, 1.000F,  0.000F, 0.365F, 1.000F,  0.000F, 0.404F, 1.000F,  0.000F, 0.443F, 1.000F,
        0.000F, 0.486F, 1.000F,  0.000F, 0.525F, 1.000F,  0.000F, 0.565F, 1.000F,  0.000F, 0.608F, 1.000F,
        0.000F, 0.647F, 1.000F,  0.000F, 0.686F, 1.000F,  0.000F, 0.725F, 1.000F,  0.000F, 0.769F, 1.000F,
        0.000F, 0.812F, 1.000F,  0.000F, 0.847F, 1.000F,  0.000F, 0.890F, 1.000F,  0.000F, 0.929F, 1.000F,
        0.000F, 0.969F, 1.000F,  0.000F, 1.000F, 0.992F,  0.000F, 1.000F, 0.949F,  0.000F, 1.000F, 0.910F,
        0.000F, 1.000F, 0.867F,  0.000F, 1.000F, 0.827F,  0.000F, 1.000F, 0.788F,  0.000F, 1.000F, 0.749F,
        0.000F, 1.000F, 0.710F,  0.000F, 1.000F, 0.667F,  0.000F, 1.000F, 0.627F,  0.000F, 1.000F, 0.588F,
        0.000F, 1.000F, 0.545F,  0.000F, 1.000F, 0.506F,  0.000F, 1.000F, 0.467F,  0.000F, 1.000F, 0.424F,
        0.000F, 1.000F, 0.384F,  0.000F, 1.000F, 0.341F,  0.000F, 1.000F, 0.302F,  0.000F, 1.000F, 0.263F,
        0.000F, 1.000F, 0.224F,  0.000F, 1.000F, 0.180F,  0.000F, 1.000F, 0.141F,  0.000F, 1.000F, 0.098F,
        0.000F, 1.000F, 0.059F,  0.000F, 1.000F, 0.020F,  0.020F, 1.000F, 0.000F,  0.059F, 1.000F, 0.000F,
        0.102F, 1.000F, 0.000F,  0.141F, 1.000F, 0.000F,  0.184F, 1.000F, 0.000F,  0.220F, 1.000F, 0.000F,
        0.263F, 1.000F, 0.000F,  0.306F, 1.000F, 0.000F,  0.345F, 1.000F, 0.000F,  0.384F, 1.000F, 0.000F,
        0.424F, 1.000F, 0.000F,  0.463F, 1.000F, 0.000F,  0.502F, 1.000F, 0.000F,  0.545F, 1.000F, 0.000F,
        0.584F, 1.000F, 0.000F,  0.627F, 1.000F, 0.000F,  0.667F, 1.000F, 0.000F,  0.706F, 1.000F, 0.000F,
        0.749F, 1.000F, 0.000F,  0.784F, 1.000F, 0.000F,  0.831F, 1.000F, 0.000F,  0.871F, 1.000F, 0.000F,
        0.910F, 1.000F, 0.000F,  0.949F, 1.000F, 0.000F,  0.988F, 1.000F, 0.000F,  1.000F, 0.973F, 0.000F,
        1.000F, 0.929F, 0.000F,  1.000F, 0.886F, 0.000F,  1.000F, 0.851F, 0.000F,  1.000F, 0.808F, 0.000F,
        1.000F, 0.769F, 0.000F,  1.000F, 0.725F, 0.000F,  1.000F, 0.686F, 0.000F,  1.000F, 0.647F, 0.000F,
        1.000F, 0.608F, 0.000F,  1.000F, 0.565F, 0.000F,  1.000F, 0.525F, 0.000F,  1.000F, 0.486F, 0.000F,
        1.000F, 0.443F, 0.000F,  1.000F, 0.404F, 0.000F,  1.000F, 0.365F, 0.000F,  1.000F, 0.325F, 0.000F,
        1.000F, 0.286F, 0.000F,  1.000F, 0.243F, 0.000F,  1.000F, 0.200F, 0.000F,  1.000F, 0.161F, 0.000F,
        1.000F, 0.122F, 0.000F,  1.000F, 0.082F, 0.000F,  1.000F, 0.039F, 0.000F,  1.000F, 0.000F, 0.000F
};

static float gray[3] = {0.5F, 0.5F, 0.5F};
static float striped[300];
static float gradient[300];

static Colormap selected = HEAT;
static float* current = heat;

static float value_min = 0;
static float value_range = 1;

/*-------------------------------- LOCAL UTILITY FUNCTIONS --------------------------------*/

// adapted from https://stackoverflow.com/questions/31127260/strcasecmp-a-non-standard-function

static int util_strcasecmp(const char *a, const char *b)
{
    int ca, cb;
    do {
        ca = (unsigned char) *a++;
        cb = (unsigned char) *b++;
        ca = tolower(toupper(ca));
        cb = tolower(toupper(cb));
	} while (ca == cb && ca != '\0');
	return ca - cb;
}

/*-------------------------------- COLORMAP FUNCTIONS --------------------------------*/

void colormap_init()
{
   for (int c = 0; c < 300; c++) {
      striped[c] = gradient[c] = -1;
  }
}

long int colormap_convert_color(const char *s)
{
	// find matching color, if any
	for (int c = 0; c < 140; c++) {
		if (util_strcasecmp(s, names[c * 2 + 1]) == 0) {
			s = names[c * 2];
			break;
		}
	}

	// 's' should be now a hex triplet
	for (size_t d = 0; d < strlen(s); d++) {
		if (!isxdigit(s[d])) {
			// fallback to 50% gray if not a valid hex string
			return 0x808080;
		}
	}
	return strtol(s, NULL, 16);
}

void colormap_use_color(int i, const char *s)
{
	if (0 <= i && i < 100) {
		long int n = colormap_convert_color(s);
		striped[i * 3    ] = ((n >> 16) & 0xff) / 255.0F;
		striped[i * 3 + 1] = ((n >> 8) & 0xff) / 255.0F;
		striped[i * 3 + 2] = (n & 0xff) / 255.0F;
	}
}

void colormap_use_rgb(int i, int r, int g, int b)
{
	if (0 <= i && i < 100) {
		striped[i * 3    ] = r / 255.0F;
		striped[i * 3 + 1] = g / 255.0F;
		striped[i * 3 + 2] = b / 255.0F;
	}
}

void colormap_generate()
{
	int a = 0, b;
	while (a < 99) {
		// set interval to [a,b)
		b = a + 1;
		while (b < 100 && striped[b * 3] == -1) {
			b++;
		}

		// copy first color to slot 0, if needed
		if (a == 0 && b != 100 && striped[0] == -1) {
			striped[0] = striped[b * 3    ];
			striped[1] = striped[b * 3 + 1];
			striped[2] = striped[b * 3 + 2];
		}

		// copy last color to slot 99, if needed
		if (b == 100) {
			striped[297] = striped[a * 3    ];
			striped[298] = striped[a * 3 + 1];
			striped[299] = striped[a * 3 + 2];
			b = 99;
		}

		// fill interval (a,b) for striped colormap
		for (int i = a + 1; i < b; i++) {
			striped[i * 3    ] = striped[a * 3    ];
			striped[i * 3 + 1] = striped[a * 3 + 1];
			striped[i * 3 + 2] = striped[a * 3 + 2];
		}

		// interpolate interval [a,b) for gradient colormap
		for (int i = a; i < b; i++) {
			float w = (float) (i - a) / (b - a);
			gradient[i * 3    ] = (1 - w) * striped[a * 3    ] + w * striped[b * 3    ];
			gradient[i * 3 + 1] = (1 - w) * striped[a * 3 + 1] + w * striped[b * 3 + 1];
			gradient[i * 3 + 2] = (1 - w) * striped[a * 3 + 2] + w * striped[b * 3 + 2];
		}

		// start next interval
		a = b;
	}

	// copy last color
	gradient[297] = striped[297];
	gradient[298] = striped[298];
	gradient[299] = striped[299];
}

void colormap_select(Colormap c)
{
	selected = c;
	switch (c) {
	case HEAT:
		current = heat;
		break;
	case STRIPED:
		current = striped;
		break;
	case GRADIENT:
		current = gradient;
		break;
	}
}

Colormap colormap_get_selected()
{
	return selected;
}

void colormap_set_limits(float min, float max)
{
	value_min = min;
	value_range = max - min;
}

float* colormap_lookup(float val)
{
	if (value_range < 0.0001) {
		// range is too small: color everything with 50% gray
		return gray;
	}
	int i = int(99 * (val - value_min) / value_range);
	if (i < 0 || i > 99) {
		return gray;
	}
	return &current[i * 3];
}
