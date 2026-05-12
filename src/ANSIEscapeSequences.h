#ifndef _ANSIESCAPESEQUENCES_H
#define _ANSIESCAPESEQUENCES_H

// Source: Wikipedia
#define ESC "\x1B"
#define CSI ESC "["

#define FAINT_START CSI "2m"
#define FAINT_END CSI "0m"

#define BOLD_START CSI "1m"
#define BOLD_END CSI "0m"

#endif
