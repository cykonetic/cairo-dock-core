/*
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __CAIRO_DOCK_SURFACE_FACTORY__
#define  __CAIRO_DOCK_SURFACE_FACTORY__

#include <glib.h>
#include <gdk/gdk.h>
#include <cairo.h>

#include "cairo-dock-struct.h"
G_BEGIN_DECLS

/**
*@file cairo-dock-surface-factory.h This class contains functions to load any image/X buffer/GdkPixbuf/text into a cairo-surface.
* The loading of an image can be modified by a mask, to take into account the ratio, zoom, orientation, etc.
*
* The general way to load an image is by using \ref cairo_dock_create_surface_from_image.
* 
* If you just want to load an image at a given size, use \ref cairo_dock_create_surface_from_image_simple, or \ref cairo_dock_create_surface_from_icon.
* 
* To load a text into a surface, describe your text look with a _GldiTextDescription, and pass it to \ref cairo_dock_create_surface_from_text.
*
* Note: if you also need to load the image into a texture, it's easier to use the higher level ImageBuffer API (see \ref cairo_dock_create_image_buffer).
*/

/// Types of image loading modifiers.
typedef enum {
	/// fill the space, with transparency if necessary.
	CAIRO_DOCK_FILL_SPACE 			= 1<<0,
	/// keep the ratio of the original image.
	CAIRO_DOCK_KEEP_RATIO 			= 1<<1,
	/// don't zoom in the image if the final surface is larger than the original image.
	CAIRO_DOCK_DONT_ZOOM_IN 		= 1<<2,
	/// orientation horizontal flip
	CAIRO_DOCK_ORIENTATION_HFLIP 		= 1<<3,
	/// orientation 180° rotation
	CAIRO_DOCK_ORIENTATION_ROT_180 	= 2<<3,
	/// orientation vertical flip
	CAIRO_DOCK_ORIENTATION_VFLIP 		= 3<<3,
	/// orientation 90° rotation + horizontal flip
	CAIRO_DOCK_ORIENTATION_ROT_90_HFLIP = 4<<3,
	/// orientation 90° rotation
	CAIRO_DOCK_ORIENTATION_ROT_90 	= 5<<3,
	/// orientation 90° rotation + vertical flip
	CAIRO_DOCK_ORIENTATION_ROT_90_VFLIP = 6<<3,
	/// orientation 270° rotation
	CAIRO_DOCK_ORIENTATION_ROT_270 	= 7<<3,
	/// load the image as a strip if possible.
	CAIRO_DOCK_ANIMATED_IMAGE = 1<<6
	} CairoDockLoadImageModifier;
/// mask to get the orientation from a CairoDockLoadImageModifier.
#define CAIRO_DOCK_ORIENTATION_MASK (7<<3)

/// Description of the rendering of a text.
struct _GldiTextDescription {
	/// font.
	gchar *cFont;
	/// pango font
	PangoFontDescription *fd;
	/// size in pixels
	gint iSize;
	/// whether to draw the decorations or not
	gboolean bNoDecorations;
	/// whether to use the default colors or the colors defined below
	gboolean bUseDefaultColors;
	/// text color
	gdouble fColorStart[3];
	/// background color
	gdouble fBackgroundColor[4];
	/// outline color
	gdouble fLineColor[4];
	/// TRUE to stroke the outline of the characters (in black).
	gboolean bOutlined;
	/// margin around the text, it is also the dimension of the frame if available.
	gint iMargin;
	/// whether to use Pango markups or not (markups are html-like marks, like <b>...</b>; using markups force you to escape some characters like "&" -> "&amp;")
	gboolean bUseMarkup;
	/// maximum width allowed, in ratio of the screen's width. Carriage returns will be inserted if necessary. 0 means no limit.
	gdouble fMaxRelativeWidth;
};


/* Calcule la taille d'une image selon une contrainte en largeur et hauteur de manière à remplir l'espace donné.
*@param fImageWidth the width of the image. Contient initialement the width of the image, et sera écrasée avec la largeur obtenue.
*@param fImageHeight the height of the image. Contient initialement the height of the image, et sera écrasée avec la hauteur obtenue.
*@param iWidthConstraint contrainte en largeur (0 <=> pas de contrainte).
*@param iHeightConstraint contrainte en hauteur (0 <=> pas de contrainte).
*@param bNoZoomUp TRUE ssi on ne doit pas agrandir the image (seulement la rétrécir).
*@param fZoomWidth sera renseigné avec le facteur de zoom en largeur qui a été appliqué.
*@param fZoomHeight sera renseigné avec le facteur de zoom en hauteur qui a été appliqué.
*/
void cairo_dock_calculate_size_fill (double *fImageWidth, double *fImageHeight, int iWidthConstraint, int iHeightConstraint, gboolean bNoZoomUp, double *fZoomWidth, double *fZoomHeight);

/* Calcule la taille d'une image selon une contrainte en largeur et hauteur en gardant le ratio hauteur/largeur constant.
*@param fImageWidth the width of the image. Contient initialement the width of the image, et sera écrasée avec la largeur obtenue.
*@param fImageHeight the height of the image. Contient initialement the height of the image, et sera écrasée avec la hauteur obtenue.
*@param iWidthConstraint contrainte en largeur (0 <=> pas de contrainte).
*@param iHeightConstraint contrainte en hauteur (0 <=> pas de contrainte).
*@param bNoZoomUp TRUE ssi on ne doit pas agrandir the image (seulement la rétrécir).
*@param fZoom sera renseigné avec le facteur de zoom qui a été appliqué.
*/
void cairo_dock_calculate_size_constant_ratio (double *fImageWidth, double *fImageHeight, int iWidthConstraint, int iHeightConstraint, gboolean bNoZoomUp, double *fZoom);


/** Calculate the size of an image according to a constraint on width and height, and a loading modifier.
*@param fImageWidth pointer to the width of the image. Initially contains the width of the original image, and is updated with the resulting width.
*@param fImageHeight pointer to the height of the image. Initially contains the height of the original image, and is updated with the resulting height.
*@param iWidthConstraint constraint on width (0 <=> no constraint).
*@param iHeightConstraint constraint on height (0 <=> no constraint).
*@param iLoadingModifier a mask of different loading modifiers.
*@param fZoomWidth will be filled with the zoom that has been applied on width.
*@param fZoomHeight will be filled with the zoom that has been applied on height.
*/
void cairo_dock_calculate_constrainted_size (double *fImageWidth, double *fImageHeight, int iWidthConstraint, int iHeightConstraint, CairoDockLoadImageModifier iLoadingModifier, double *fZoomWidth, double *fZoomHeight);

/** Create a surface from raw data of an X icon. The biggest icon possible is taken. The ratio is kept, and the surface will fill the space with transparency if necessary.
*@param pXIconBuffer raw data of the icon.
*@param iBufferNbElements number of elements in the buffer.
*@param iWidth will be filled with the resulting width of the surface.
*@param iHeight will be filled with the resulting height of the surface.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_xicon_buffer (gulong *pXIconBuffer, int iBufferNbElements, int iWidth, int iHeight);

/** Create a surface from a GdkPixbuf.
*@param pixbuf the pixbuf.
*@param fMaxScale maximum zoom of the icon.
*@param iWidthConstraint constraint on the width, or 0 to not constraint it.
*@param iHeightConstraint constraint on the height, or 0 to not constraint it.
*@param iLoadingModifier a mask of different loading modifiers.
*@param fImageWidth will be filled with the resulting width of the surface (hors zoom).
*@param fImageHeight will be filled with the resulting height of the surface (hors zoom).
*@param fZoomX if non NULL, will be filled with the zoom that has been applied on width.
*@param fZoomY if non NULL, will be filled with the zoom that has been applied on width.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_pixbuf (GdkPixbuf *pixbuf, double fMaxScale, int iWidthConstraint, int iHeightConstraint, CairoDockLoadImageModifier iLoadingModifier, double *fImageWidth, double *fImageHeight, double *fZoomX, double *fZoomY);


void cairo_dock_reset_source_context (void);

/** Create an empty surface (transparent) of a given size. In OpenGL mode, this surface can act as a buffer to generate a texture.
*@param iWidth width of the surface.
*@param iHeight height of the surface.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_blank_surface (int iWidth, int iHeight);

/** Create a surface from any image.
*@param cImagePath complete path to the image.
*@param fMaxScale maximum zoom of the icon.
*@param iWidthConstraint constraint on the width, or 0 to not constraint it.
*@param iHeightConstraint constraint on the height, or 0 to not constraint it.
*@param iLoadingModifier a mask of different loading modifiers.
*@param fImageWidth will be filled with the resulting width of the surface (hors zoom).
*@param fImageHeight will be filled with the resulting height of the surface (hors zoom).
*@param fZoomX if non NULL, will be filled with the zoom that has been applied on width.
*@param fZoomY if non NULL, will be filled with the zoom that has been applied on width.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_image (const gchar *cImagePath, double fMaxScale, int iWidthConstraint, int iHeightConstraint, CairoDockLoadImageModifier iLoadingModifier, double *fImageWidth, double *fImageHeight, double *fZoomX, double *fZoomY);

/** Create a surface from any image, at a given size. If the image is given by its sole name, it is searched inside the current theme root folder.
*@param cImageFile path or name of an image.
*@param fImageWidth the desired surface width.
*@param fImageHeight the desired surface height.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_image_simple (const gchar *cImageFile, double fImageWidth, double fImageHeight);

/** Create a surface from any image, at a given size. If the image is given by its sole name, it is searched inside the icons themes known by Cairo-Dock. 
*@param cImagePath path or name of an image.
*@param fImageWidth the desired surface width.
*@param fImageHeight the desired surface height.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_icon (const gchar *cImagePath, double fImageWidth, double fImageHeight);
#define cairo_dock_create_surface_for_icon cairo_dock_create_surface_from_icon

/** Create a square surface from any image, at a given size. If the image is given by its sole name, it is searched inside the icons themes known by Cairo-Dock.
*@param cImagePath path or name of an image.
*@param fImageSize the desired surface size.
*@return the newly allocated surface.
*/
#define cairo_dock_create_surface_for_square_icon(cImagePath, fImageSize) cairo_dock_create_surface_for_icon (cImagePath, fImageSize, fImageSize)


/** Create a surface at a given size, and fill it with a pattern. If the pattern image is given by its sole name, it is searched inside the current theme root folder.
*@param cImageFile path or name of an image that will be repeated to fill the surface.
*@param fImageWidth the desired surface width.
*@param fImageHeight the desired surface height.
*@param fAlpha transparency of the pattern (1 means opaque).
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_pattern (const gchar *cImageFile, double fImageWidth, double fImageHeight, double fAlpha);


/** Create a surface by rotating another. Only works for 1/4 of rounds.
*@param pSurface surface to rotate.
*@param fImageWidth the width of the surface.
*@param fImageHeight the height of the surface.
*@param fRotationAngle rotation angle to apply, in radians.
*@return the newly allocated surface.
*/
cairo_surface_t * cairo_dock_rotate_surface (cairo_surface_t *pSurface, double fImageWidth, double fImageHeight, double fRotationAngle);

/** Create a surface representing a text, according to a given text description.
*@param cText the text.
*@param pLabelDescription description of the text rendering.
*@param fMaxScale maximum zoom of the text.
*@param iMaxWidth maximum authorized width for the surface; it will be zoomed in to fits this limit. 0 for no limit.
*@param iTextWidth will be filled the width of the resulting surface.
*@param iTextHeight will be filled the height of the resulting surface.
*@return the newly allocated surface.
*/
cairo_surface_t *cairo_dock_create_surface_from_text_full (const gchar *cText, GldiTextDescription *pLabelDescription, double fMaxScale, int iMaxWidth, int *iTextWidth, int *iTextHeight);

/** Create a surface representing a text, according to a given text description.
*@param cText the text.
*@param pLabelDescription description of the text rendering.
*@param iTextWidthPtr will be filled the width of the resulting surface.
*@param iTextHeightPtr will be filled the height of the resulting surface.
*@return the newly allocated surface.
*/
#define cairo_dock_create_surface_from_text(cText, pLabelDescription, iTextWidthPtr, iTextHeightPtr) cairo_dock_create_surface_from_text_full (cText, pLabelDescription, 1., 0, iTextWidthPtr, iTextHeightPtr) 

/** Create a surface identical to another, possibly resizing it.
*@param pSurface surface to duplicate.
*@param fWidth the width of the surface.
*@param fHeight the height of the surface.
*@param fDesiredWidth desired width of the copy (0 to keep the same size).
*@param fDesiredHeight desired height of the copy (0 to keep the same size).
*@return the newly allocated surface.
*/
cairo_surface_t * cairo_dock_duplicate_surface (cairo_surface_t *pSurface, double fWidth, double fHeight, double fDesiredWidth, double fDesiredHeight);



void gldi_text_description_free (GldiTextDescription *pTextDescription);
void gldi_text_description_copy (GldiTextDescription *pDestTextDescription, GldiTextDescription *pOrigTextDescription);
GldiTextDescription *gldi_text_description_duplicate (GldiTextDescription *pTextDescription);

void gldi_text_description_reset (GldiTextDescription *pTextDescription);

void gldi_text_description_set_font (GldiTextDescription *pTextDescription, gchar *cFont);

#define gldi_text_description_get_size(pTextDescription) (pTextDescription)->iSize

#define gldi_text_description_get_description(pTextDescription) (pTextDescription)->fd


G_END_DECLS
#endif
