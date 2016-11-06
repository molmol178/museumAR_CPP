/*
 * filename : anytopnm.c
 * author   : Takahiro Sugiyama
 * date     : Friday, November 10 1995
 * describe : Any Image Type translate into PNM type.
 */


#include "Funcs.h"
#include <math.h>
#include <stdlib.h>


#define DEFINE_ANY_TO_PNM_0D(TYPE) \
static void any_to_pnm_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  Image.copy(output, input);\
}


#define DEFINE_ANY_TO_PNM_1D(TYPE) \
static void any_to_pnm_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  register TYPE **src;\
  register uchar **data;\
  register long xsize, ysize;\
  register int x, y;\
  register uchar *u;\
\
  long minus;\
  double p, mag_max, mag;\
\
  if (getenv("CIL_COLORED_LABEL"))\
    {\
      any_label_to_##TYPE(input, output);\
      return;\
    }\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar, xsize, ysize);\
  data = __DATA(output, uchar);\
\
  mag_max = src[0][0];\
  minus = 0;\
  for ( y = 0; y < ysize; y++ )\
    for ( x = 0; x < xsize; x++ )\
      {\
	p = src[y][x];\
	if (p < 0) minus = 1;\
	mag = fabs(p);\
	if (mag_max < mag) mag_max = mag;\
      }\
\
  for ( y = 0; y < ysize; y++ )\
    for ( x = 0; x < xsize; x++ )\
      {\
	p = src[y][x];\
\
	if (minus)\
	  {\
	    data[y][x] = 127 * p / mag_max + 127;\
	  }\
	else\
	  {\
	    data[y][x] = 255 * p / mag_max;\
	  }\
      }\
}



#define DEFINE_ANY_TO_PNM_2D(TYPE) \
static void any_to_pnm_##TYPE\
  _P2 (( image, output  ),\
       ( image, input ))\
{\
  register TYPE **src;\
  register uchar3 **data;\
  register long xsize, ysize;\
  register int x, y;\
  register uchar *u;\
\
  double px, py;\
  double mag, mag_max;\
\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar3, xsize, ysize);\
  data = __DATA(output, uchar3);\
  mag_max = 0;\
  for (y = 0; y < ysize; y++)\
    for (x = 0; x < xsize; x++)\
      {\
	px = src[y][x].at[0];\
	py = src[y][x].at[1];\
	mag = px*px + py*py;\
	if (mag_max < mag) mag_max = mag;\
      }\
  mag_max = sqrt(mag_max);\
\
  for (y = 0; y < ysize; y++)\
    for (x = 0; x < xsize; x++)\
      {\
	double r, g, b;\
	double vx, vy;\
	double theta, radius;\
\
	px = src[y][x].at[0];\
	py = src[y][x].at[1];\
	u = data[y][x].at;\
\
	vx = - px / mag_max;\
	vy = py / mag_max;\
\
	if ((vy == 0) && (vx == 0))\
	  {\
	    u[0] = 255;\
	    u[1] = 255;\
	    u[2] = 255;\
	    continue;\
	  }\
	theta = (vy == 0) ? ((vx>0)?0:M_PI) : atan2(vy, vx);\
	if (theta < 0) theta = 2*M_PI + theta;\
	radius = sqrt(vx*vx+vy*vy);\
	if (theta <= 2*M_PI/3)\
	  {\
	    b = (1.0 - radius) / 1.0;\
	    r = radius*(cos(theta) + sin(theta) / tan(M_PI/3.0)) + b;\
	    g = radius*(cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0)) + b;\
	  }\
	else\
	if (4*M_PI/3 <= theta) /* R--B */\
	  {\
	    theta -= 4*M_PI/3;\
	    g = (1.0 - radius) / 1.0;\
	    b = radius*(cos(theta) + sin(theta) / tan(M_PI/3.0)) + g;\
	    r = radius*(cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0)) + g;\
	  }\
	else /* B--G */\
	  {\
	    theta -= 2*M_PI/3;\
	    r = (1.0 - radius) / 1.0;\
	    g = radius*(cos(theta) + sin(theta) / tan(M_PI/3.0)) + r;\
	    b = radius*(cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0)) + r;\
	  }\
\
	r = 255 * r; if (r > 255) r = 255; else if (r < 0) r = 0;\
	g = 255 * g; if (g > 255) g = 255; else if (g < 0) g = 0;\
	b = 255 * b; if (b > 255) b = 255; else if (b < 0) b = 0;\
\
	u[0] = r;\
	u[1] = g;\
	u[2] = b;\
      }\
}



#define DEFINE_ANY_TO_PNM_3D(TYPE) \
static void any_to_pnm_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  register TYPE **src;\
  register uchar3 **data;\
  register long xsize, ysize;\
  register int x, y;\
\
  long minus;\
  double p0, p1, p2;\
  double mag_max, mag_min, mag_range, mag, mag_offset;\
\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar3, xsize, ysize);\
  data = __DATA(output, uchar3);\
\
  p0 = src[0][0].at[0];\
  p1 = src[0][0].at[1];\
  p2 = src[0][0].at[2];\
  mag_max = mag_min = p0*p0 + p1*p1 + p2*p2;\
  minus = 0;\
  for ( y = 0; y < ysize; y++ )\
    for ( x = 0; x < xsize; x++ )\
      {\
	p0 = src[y][x].at[0];\
	p1 = src[y][x].at[1];\
	p2 = src[y][x].at[2];\
	if ((p0 < 0) || (p1 < 0) || (p2 < 0)) minus = 1;\
	mag = p0*p0 + p1*p1 + p2*p2;\
	if (mag_max < mag) mag_max = mag; else\
	if (mag_min > mag) mag_min = mag;\
      }\
  mag_max = sqrt(mag_max);\
  mag_min = sqrt(mag_min);\
  mag_range = mag_max - mag_min;\
\
  if (minus)\
    {\
      mag_max = 127.0*sqrt(3.0)/mag_range;\
      mag_offset = 127;\
    }\
  else\
    {\
      mag_max = 255.0*sqrt(3.0)/mag_range;\
      mag_offset = 0;\
    }\
  mag_min = mag_max*mag_min;\
\
  for ( y = 0; y < ysize; y++ )\
    for ( x = 0; x < xsize; x++ )\
      {\
	p0 = src[y][x].at[0];\
	p1 = src[y][x].at[1];\
	p2 = src[y][x].at[2];\
\
	mag = sqrt(p0*p0 + p1*p1 + p2*p2);\
	if (mag != 0) mag = mag_max - mag_min/mag;\
	p0 = p0 * mag + mag_offset;\
	p1 = p1 * mag + mag_offset;\
	p2 = p2 * mag + mag_offset;\
\
	if (p0 > 255) p0 = 255; else if (p0 < 0) p0 = 0;\
	if (p1 > 255) p1 = 255; else if (p1 < 0) p1 = 0;\
	if (p2 > 255) p2 = 255; else if (p2 < 0) p2 = 0;\
\
        data[y][x].at[0] = p0;\
	data[y][x].at[1] = p1;\
	data[y][x].at[2] = p2;\
      }\
}



/*
 * ラベルのカラーマップの設定
 */

uchar3 *image__make_label_colormap _P1 ((long, label_max))
{
  long max_color, smax, reverse = 0;
  char *env = getenv("CIL_COLORED_LABEL");
  uchar3 *cmap;
  long x;

  if (env == 0) return 0;
  max_color = 0;
  if (strcmp(env,"all") == 0)
    max_color = label_max;
  else
  if (strcmp(env,"feature") == 0)
    {reverse = 1; max_color = 1.5*label_max - 1;}
  else
  if (strlen(env) >= 1) max_color = atoi(env);
  if (max_color <= 0) max_color = 12;
  max_color += 1;

  cmap = typenew1(label_max + 3, uchar3);
  smax = label_max / max_color + 1;
  for (x = 1; x <= label_max; x++)
    {
      double theta;
      double s;
      int ti, si;
      double r,g,b;

      si = x / max_color;
      s = 1 - (double)si / smax;

      ti = (x - 1) % max_color;
      theta = 2 * M_PI * (ti + (double)si/smax) / max_color;

      if (theta <= 2*M_PI/3) /* R--G */
	{
	  b = 0;
	  r = cos(theta) + sin(theta) / tan(M_PI/3.0);
	  g = cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0);
	  if (s < 0.5) { b += s; } else { r *= s; g *= s; }
	}
      else
      if (4*M_PI/3 <= theta) /* R--B */
	{
	  theta -= 4*M_PI/3;
	  g = 0;
	  b = cos(theta) + sin(theta) / tan(M_PI/3.0);
	  r = cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0);
	  if (s < 0.5) { g += s; } else { r *= s; b *= s; }
	}
      else /* B--G */
	{
	  theta -= 2*M_PI/3;
	  r = 0;
	  g = cos(theta) + sin(theta) / tan(M_PI/3.0);
	  b = cos(2*M_PI/3.0-theta) + sin(2*M_PI/3.0-theta) / tan(M_PI/3.0);
	  if (s < 0.5) { r += s; } else { b *= s; g *= s; }
	}
      r = 255 * r;
      g = 255 * g;
      b = 255 * b;

      if (r > 255) r = 255; else if (r < 0) r = 0;
      if (g > 255) g = 255; else if (g < 0) g = 0;
      if (b > 255) b = 255; else if (b < 0) b = 0;

      if (reverse)
	{
	  cmap[x].at[0] = b;
	  cmap[x].at[1] = g;
	  cmap[x].at[2] = r;
	}
      else
	{
	  cmap[x].at[0] = r;
	  cmap[x].at[1] = g;
	  cmap[x].at[2] = b;
	}
    }
  return cmap;
}



#define DEFINE_ANY_UNSIGNED_LABEL_TO_PNM(TYPE) \
static void any_label_to_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  register TYPE **src;\
  register uchar3 **data;\
  register long xsize, ysize;\
  register int x, y, sx, sy, xx, yy, px, py;\
  register unsigned long black, white;\
  register long label;\
  register uchar3 *cmap;\
  long label_max;\
\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar3, xsize, ysize);\
  data = __DATA(output, uchar3);\
\
  if (__TYPE(input) == UChar)\
    label_max = 255;\
  else\
    {\
      label_max = 0;\
      for (y = 0; y < ysize; y++)\
	for (x = 0; x < xsize; x++)\
	  if (src[y][x] > label_max)\
	    label_max = src[y][x];\
    }\
\
  cmap = image__make_label_colormap(label_max);\
\
  for ( y = 0; y < ysize; y++)\
    for ( x = 0; x < xsize; x++ )\
      {\
	label = src[y][x];\
\
	if ( label == 0 )\
	  {\
	    data[y][x].at[0] = 100;\
	    data[y][x].at[1] = 100;\
	    data[y][x].at[2] = 100;\
	  }\
	else\
	  {\
	    data[y][x] = cmap[label];\
	  }\
      }\
  typefree1(cmap);\
}



#define DEFINE_ANY_SIGNED_LABEL_TO_PNM(TYPE) \
static void any_label_to_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  register TYPE **src;\
  register uchar3 **data;\
  register long xsize, ysize;\
  register int x, y, sx, sy, xx, yy, px, py;\
  register unsigned long black, white;\
  register long label;\
  register uchar3 *cmap;\
  long label_max;\
\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar3, xsize, ysize);\
  data = __DATA(output, uchar3);\
\
  if (__TYPE(input) == Char)\
    label_max = 255;\
  else\
    {\
      label_max = 0;\
      for (y = 0; y < ysize; y++)\
	for (x = 0; x < xsize; x++)\
	  if (src[y][x] > label_max)\
	    label_max = src[y][x];\
    }\
\
  cmap = image__make_label_colormap(label_max);\
\
  for ( y = 0; y < ysize; y++)\
    for ( x = 0; x < xsize; x++ )\
      {\
	label = src[y][x];\
\
	if ( label == 0 )\
	  {\
	    data[y][x].at[0] = 100;\
	    data[y][x].at[1] = 100;\
	    data[y][x].at[2] = 100;\
	  }\
	else\
	if ( label < 0 )\
	  {\
	    if (__TYPE(input) != Char)\
	      {\
		data[y][x].at[0] = 50;\
		data[y][x].at[1] = 50;\
		data[y][x].at[2] = 50;\
	      }\
	    else\
	      {\
		data[y][x] = cmap[256 - label];\
	      }\
	  }\
	else\
	  {\
	    data[y][x] = cmap[label];\
	  }\
      }\
  typefree1(cmap);\
}



#define DEFINE_ANY_REAL_LABEL_TO_PNM(TYPE) \
static void any_label_to_##TYPE\
  _P2 ((image, output),\
       (image, input))\
{\
  register TYPE **src;\
  register uchar3 **data;\
  register long xsize, ysize;\
  register int x, y, sx, sy, xx, yy, px, py;\
  register unsigned long black, white;\
  register long label;\
  register uchar3 *cmap;\
  double value_max, value_min;\
\
  src = __DATA(input, TYPE);\
  xsize = __XSIZE(input);\
  ysize = __YSIZE(input);\
\
  Image.make(output, UChar3, xsize, ysize);\
  data = __DATA(output, uchar3);\
\
  value_max = 0;\
  value_min = 0;\
  for (y = 0; y < ysize; y++)\
    for (x = 0; x < xsize; x++)\
      {\
	if (src[y][x] > value_max) value_max = src[y][x]; else\
	if (src[y][x] < value_min) value_min = src[y][x];\
      }\
\
  cmap = image__make_label_colormap(1025);\
\
  for ( y = 0; y < ysize; y++)\
    for ( x = 0; x < xsize; x++ )\
      {\
	label = 1024*(src[y][x] - value_min) / (value_max - value_min) + 1;\
\
	if ( label == 0 )\
	  {\
	    data[y][x].at[0] = 100;\
	    data[y][x].at[1] = 100;\
	    data[y][x].at[2] = 100;\
	  }\
	else\
	  {\
	    data[y][x] = cmap[label];\
	  }\
      }\
  typefree1(cmap);\
}



DEFINE_ANY_TO_PNM_0D(packedbit1)
DEFINE_ANY_TO_PNM_0D(bit1)

DEFINE_ANY_UNSIGNED_LABEL_TO_PNM(bit4)
DEFINE_ANY_SIGNED_LABEL_TO_PNM(char)
DEFINE_ANY_SIGNED_LABEL_TO_PNM(short)
DEFINE_ANY_SIGNED_LABEL_TO_PNM(long)
DEFINE_ANY_REAL_LABEL_TO_PNM(float)
DEFINE_ANY_REAL_LABEL_TO_PNM(double)
DEFINE_ANY_UNSIGNED_LABEL_TO_PNM(uchar)
DEFINE_ANY_UNSIGNED_LABEL_TO_PNM(ushort)
DEFINE_ANY_UNSIGNED_LABEL_TO_PNM(ulong)

DEFINE_ANY_TO_PNM_1D(bit4)
DEFINE_ANY_TO_PNM_1D(char)
DEFINE_ANY_TO_PNM_1D(short)
DEFINE_ANY_TO_PNM_1D(long)
DEFINE_ANY_TO_PNM_1D(float)
DEFINE_ANY_TO_PNM_1D(double)
DEFINE_ANY_TO_PNM_1D(uchar)
DEFINE_ANY_TO_PNM_1D(ushort)
DEFINE_ANY_TO_PNM_1D(ulong)

DEFINE_ANY_TO_PNM_2D(char2)
DEFINE_ANY_TO_PNM_2D(short2)
DEFINE_ANY_TO_PNM_2D(long2)
DEFINE_ANY_TO_PNM_2D(float2)
DEFINE_ANY_TO_PNM_2D(double2)
DEFINE_ANY_TO_PNM_2D(uchar2)
DEFINE_ANY_TO_PNM_2D(ushort2)
DEFINE_ANY_TO_PNM_2D(ulong2)

DEFINE_ANY_TO_PNM_3D(char3)
DEFINE_ANY_TO_PNM_3D(short3)
DEFINE_ANY_TO_PNM_3D(long3)
DEFINE_ANY_TO_PNM_3D(float3)
DEFINE_ANY_TO_PNM_3D(double3)
DEFINE_ANY_TO_PNM_0D(uchar3)
DEFINE_ANY_TO_PNM_3D(ushort3)
DEFINE_ANY_TO_PNM_3D(ulong3)



typedef void (*f_caller)
     P2 (( image, output ),
	 ( image, input ))


static caller_table table[] = {
  {PackedBit1, any_to_pnm_packedbit1},
  {Bit1, any_to_pnm_bit1},
  {Bit4, any_to_pnm_bit4},

  {Char, any_to_pnm_char},
  {Short, any_to_pnm_short},
  {Long, any_to_pnm_long},
  {Float, any_to_pnm_float},
  {Double, any_to_pnm_double},
  {UChar, any_to_pnm_uchar},
  {UShort, any_to_pnm_ushort},
  {ULong, any_to_pnm_ulong},

  {Char2, any_to_pnm_char2},
  {Short2, any_to_pnm_short2},
  {Long2, any_to_pnm_long2},
  {Float2, any_to_pnm_float2},
  {Double2, any_to_pnm_double2},
  {UChar2, any_to_pnm_uchar2},
  {UShort2, any_to_pnm_ushort2},
  {ULong2, any_to_pnm_ulong2},

  {Char3, any_to_pnm_char3},
  {Short3, any_to_pnm_short3},
  {Long3, any_to_pnm_long3},
  {Float3, any_to_pnm_float3},
  {Double3, any_to_pnm_double3},
  {UChar3, any_to_pnm_uchar3},
  {UShort3, any_to_pnm_ushort3},
  {ULong3, any_to_pnm_ulong3},
};



void image__any_to_pnm
  _P2 (( image, output ),
       ( image, input ))
{
  f_caller caller;

  caller = (f_caller)image__caller( "image__any_to_pnm", input, Number(table), table );

  if ( caller ) caller( output, input );
}
