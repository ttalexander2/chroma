#pragma once

//Adapted from glfw3.h

/// PRINTABLE KEYS
#define CHROMA_KEY_SPACE              32
#define CHROMA_KEY_APOSTROPHE         39  /* ' */
#define CHROMA_KEY_COMMA              44  /* , */
#define CHROMA_KEY_MINUS              45  /* - */
#define CHROMA_KEY_PERIOD             46  /* . */
#define CHROMA_KEY_SLASH              47  /* / */
#define CHROMA_KEY_0                  48
#define CHROMA_KEY_1                  49
#define CHROMA_KEY_2                  50
#define CHROMA_KEY_3                  51
#define CHROMA_KEY_4                  52
#define CHROMA_KEY_5                  53
#define CHROMA_KEY_6                  54
#define CHROMA_KEY_7                  55
#define CHROMA_KEY_8                  56
#define CHROMA_KEY_9                  57
#define CHROMA_KEY_SEMICOLON          59  /* ; */
#define CHROMA_KEY_EQUAL              61  /* = */
#define CHROMA_KEY_A                  65
#define CHROMA_KEY_B                  66
#define CHROMA_KEY_C                  67
#define CHROMA_KEY_D                  68
#define CHROMA_KEY_E                  69
#define CHROMA_KEY_F                  70
#define CHROMA_KEY_G                  71
#define CHROMA_KEY_H                  72
#define CHROMA_KEY_I                  73
#define CHROMA_KEY_J                  74
#define CHROMA_KEY_K                  75
#define CHROMA_KEY_L                  76
#define CHROMA_KEY_M                  77
#define CHROMA_KEY_N                  78
#define CHROMA_KEY_O                  79
#define CHROMA_KEY_P                  80
#define CHROMA_KEY_Q                  81
#define CHROMA_KEY_R                  82
#define CHROMA_KEY_S                  83
#define CHROMA_KEY_T                  84
#define CHROMA_KEY_U                  85
#define CHROMA_KEY_V                  86
#define CHROMA_KEY_W                  87
#define CHROMA_KEY_X                  88
#define CHROMA_KEY_Y                  89
#define CHROMA_KEY_Z                  90
#define CHROMA_KEY_LEFT_BRACKET       91  /* [ */
#define CHROMA_KEY_BACKSLASH          92  /* \ */
#define CHROMA_KEY_RIGHT_BRACKET      93  /* ] */
#define CHROMA_KEY_GRAVE_ACCENT       96  /* ` */
#define CHROMA_KEY_WORLD_1            161 /* non-US #1 */
#define CHROMA_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define CHROMA_KEY_ESCAPE             256
#define CHROMA_KEY_ENTER              257
#define CHROMA_KEY_TAB                258
#define CHROMA_KEY_BACKSPACE          259
#define CHROMA_KEY_INSERT             260
#define CHROMA_KEY_DELETE             261
#define CHROMA_KEY_RIGHT              262
#define CHROMA_KEY_LEFT               263
#define CHROMA_KEY_DOWN               264
#define CHROMA_KEY_UP                 265
#define CHROMA_KEY_PAGE_UP            266
#define CHROMA_KEY_PAGE_DOWN          267
#define CHROMA_KEY_HOME               268
#define CHROMA_KEY_END                269
#define CHROMA_KEY_CAPS_LOCK          280
#define CHROMA_KEY_SCROLL_LOCK        281
#define CHROMA_KEY_NUM_LOCK           282
#define CHROMA_KEY_PRINT_SCREEN       283
#define CHROMA_KEY_PAUSE              284
#define CHROMA_KEY_F1                 290
#define CHROMA_KEY_F2                 291
#define CHROMA_KEY_F3                 292
#define CHROMA_KEY_F4                 293
#define CHROMA_KEY_F5                 294
#define CHROMA_KEY_F6                 295
#define CHROMA_KEY_F7                 296
#define CHROMA_KEY_F8                 297
#define CHROMA_KEY_F9                 298
#define CHROMA_KEY_F10                299
#define CHROMA_KEY_F11                300
#define CHROMA_KEY_F12                301
#define CHROMA_KEY_F13                302
#define CHROMA_KEY_F14                303
#define CHROMA_KEY_F15                304
#define CHROMA_KEY_F16                305
#define CHROMA_KEY_F17                306
#define CHROMA_KEY_F18                307
#define CHROMA_KEY_F19                308
#define CHROMA_KEY_F20                309
#define CHROMA_KEY_F21                310
#define CHROMA_KEY_F22                311
#define CHROMA_KEY_F23                312
#define CHROMA_KEY_F24                313
#define CHROMA_KEY_F25                314
#define CHROMA_KEY_KP_0               320
#define CHROMA_KEY_KP_1               321
#define CHROMA_KEY_KP_2               322
#define CHROMA_KEY_KP_3               323
#define CHROMA_KEY_KP_4               324
#define CHROMA_KEY_KP_5               325
#define CHROMA_KEY_KP_6               326
#define CHROMA_KEY_KP_7               327
#define CHROMA_KEY_KP_8               328
#define CHROMA_KEY_KP_9               329
#define CHROMA_KEY_KP_DECIMAL         330
#define CHROMA_KEY_KP_DIVIDE          331
#define CHROMA_KEY_KP_MULTIPLY        332
#define CHROMA_KEY_KP_SUBTRACT        333
#define CHROMA_KEY_KP_ADD             334
#define CHROMA_KEY_KP_ENTER           335
#define CHROMA_KEY_KP_EQUAL           336
#define CHROMA_KEY_LEFT_SHIFT         340
#define CHROMA_KEY_LEFT_CONTROL       341
#define CHROMA_KEY_LEFT_ALT           342
#define CHROMA_KEY_LEFT_SUPER         343
#define CHROMA_KEY_RIGHT_SHIFT        344
#define CHROMA_KEY_RIGHT_CONTROL      345
#define CHROMA_KEY_RIGHT_ALT          346
#define CHROMA_KEY_RIGHT_SUPER        347
#define CHROMA_KEY_MENU               348

#define CHROMA_KEY_LAST               CHROMA_KEY_MENU