#ifndef _ANSIESCAPESEQUENCES_H
#define _ANSIESCAPESEQUENCES_H

// Source: Wikipedia
#define ESC "\x1B"
#define CSI ESC "["

// D suffix denoting a signed integer displayed as decimal
#define BOLDD CSI "1m%d" CSI "0m"
#define FAINTD CSI "2m%d" CSI "0m"

#define CUR_GO_UP CSI "%dA"
#define CUR_GO_DOWN CSI "%dB"
#define CUR_GO_RIGHT CSI "%dC"
#define CUR_GO_LEFT CSI "%dD"

// Those are private sequences, hopefully they work
// (TODO maybe, but not really because mainly for debugging purposes)
#define CUR_SAVE_POSITION CSI "s"
#define CUR_RESTORE_POSITION CSI "u"

#endif
