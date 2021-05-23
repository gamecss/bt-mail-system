#ifndef	courier_unicode_h
#define	courier_unicode_h

/*
** Copyright 2000-2021 Double Precision, Inc.
** See COPYING for distribution information.
**
*/

#ifdef	__cplusplus

#include <string>
#include <vector>
#include <list>
#include <functional>
#include <tuple>

extern "C" {
#endif

#if 0
}
#endif

#include	<stdlib.h>
#include	<stdio.h>
#include	<stdint.h>
#include	<sys/types.h>
#include	<locale.h>

#ifdef __cplusplus

#else

#if 1

#include	<uchar.h>

#else

typedef uint32_t char32_t;
#endif
#endif

#define COURIER_UNICODE_VERSION 2002003

/*
** The system default character set, from the locale.
*/

extern const char *unicode_default_chset();

/*
** The current locale character set.
*/

extern const char *unicode_locale_chset();

#if 1
extern const char *unicode_locale_chset_l(locale_t l);
#endif

/* Unicode upper/lower/title case conversion functions */

extern char32_t unicode_uc(char32_t);
extern char32_t unicode_lc(char32_t);
extern char32_t unicode_tc(char32_t);

/*
** Look up HTML 4.0/XHTML entity.
**
** n="amp", etc...
**
** Returns the unicode entity value, or 0 if no such entity is defined.
*/

char32_t unicode_html40ent_lookup(const char *n);

/*
** East Asian Width lookup.
**
** unicode_eastasia looks up the EastAsianWidth property for the given
** Unicode character.
*/

#define UNICODE_EASTASIA_A	'A'	/* Ambiguous */
#define UNICODE_EASTASIA_F	'F'	/* Full width */
#define UNICODE_EASTASIA_H	'H'	/* Half width */
#define UNICODE_EASTASIA_N	'/'	/* Unassigned */
#define UNICODE_EASTASIA_Na	'N'	/* Narrow */
#define UNICODE_EASTASIA_W	'W'	/* Wide */

typedef char unicode_eastasia_t;

unicode_eastasia_t unicode_eastasia(char32_t);

/*
**
** Return "width" of unicode character.
**
** This is defined as follows: for characters having the F or W property in
** tr11 (EastAsianWidth), unicode_wcwidth() returns 2.
**
** Otherwise, characters having the BK, CR, LF, CM, NL, WJ, and ZW line
** breaking property as per tr14, unicode_wcwdith() returns 0. For all other
** cases, 1.
**
** This provides a rough estimate of the "width" of the character if its
** shown on a text console.
*/

extern int unicode_wcwidth(char32_t c);
extern size_t unicode_wcwidth_str(const char32_t *c);

/* Internal unicode table lookup functions */

extern uint8_t unicode_tab_lookup(char32_t ch,
				  const size_t *unicode_starting_indextab,
				  const char32_t *unicode_starting_pagetab,
				  size_t unicode_tab_sizeof,
				  const uint8_t (*unicode_rangetab)[2],
				  size_t unicode_rangetab_sizeof,
				  const uint8_t *unicode_classtab,
				  uint8_t uclass);

extern uint32_t unicode_tab32_lookup(char32_t ch,
				     const size_t *unicode_starting_indextab,
				     const char32_t *unicode_starting_pagetab,
				     size_t unicode_tab_sizeof,
				     const uint8_t (*unicode_rangetab)[2],
				     size_t unicode_rangetab_sizeof,
				     const uint32_t *unicode_classtab,
				     uint32_t uclass);

/*
** Look up unicode categorization, see http://unicode.org/notes/tn36/
**
** Returns a 32 bit value with four unicode categories encoded in the
** bits defined by UNICODE_CATEGORY_1..4
*/

#define UNICODE_CATEGORY_1   0xFF000000
#define UNICODE_CATEGORY_2   0x00FF0000
#define UNICODE_CATEGORY_3   0x0000FF00
#define UNICODE_CATEGORY_4   0x000000FF

#include <courier-unicode-categories-tab.h>

uint32_t unicode_category_lookup(char32_t);

/*
** Return non-0 for TAB, and all UNICODE_CATEGORY_2_SPACE.
*/

extern int unicode_isblank(char32_t ch);

/*
** The unicode-ish isspace(). In addition to return non-0 for
** unicode_isblank(), this also returns non-0 for unicode characters
** with linebreaking properties of BK, CR, LF, NL, and SP.
*/
extern int unicode_isspace(char32_t ch);

/*
** Return non-0 for all UNICODE_CATEGORY_1_LETTER
*/

extern int unicode_isalpha(char32_t ch);

/*
** Return non-0 for all UNICODE_CATEGORY_1_NUMBER | UNICODE_CATEGORY_2_DIGIT,
** only (no third categories).
*/
extern int unicode_isdigit(char32_t ch);

/*
** Return non-0 for all unicode_isalpha() or unicode_isdigit().
*/

extern int unicode_isalnum(char32_t ch);

/*
** Returns non-0 for all codepoints above SPACE which are not
** unicode_isspace().
*/

extern int unicode_isgraph(char32_t ch);

/*
** Return non-0 for all UNICODE_CATEGORY_1_PUNCTUATION.
*/

extern int unicode_ispunct(char32_t ch);

/*
** Return non-0 for all unicode_isalpha() for which the character is
** equal to unicode_lc() of itself.
*/
extern int unicode_islower(char32_t ch);

/*
** Return non-0 for all unicode_isalpha() for which the character is
** equal to unicode_uc() of itself.
*/
extern int unicode_isupper(char32_t ch);

/*
** Implementation of Unicode emoji classification, as per
** http://www.unicode.org/reports/tr51/tr51-18.html
**
** Given a char32_t, returns the character's emoji value, which is a bitmask:
**
*/

#define UNICODE_EMOJI_NONE			0
#define UNICODE_EMOJI				1
#define UNICODE_EMOJI_PRESENTATION		2
#define UNICODE_EMOJI_MODIFIER			4
#define UNICODE_EMOJI_MODIFIER_BASE		8
#define UNICODE_EMOJI_COMPONENT			16
#define UNICODE_EMOJI_EXTENDED_PICTOGRAPHIC	32

typedef unsigned char unicode_emoji_t;

extern unicode_emoji_t unicode_emoji_lookup(char32_t);

/* Look up just one of the properties, returns non-0 if the char has it */

extern int unicode_emoji(char32_t);
extern int unicode_emoji_presentation(char32_t);
extern int unicode_emoji_modifier(char32_t);
extern int unicode_emoji_modifier_base(char32_t);
extern int unicode_emoji_component(char32_t);
extern int unicode_emoji_extended_pictographic(char32_t);

/*
** Implementation of grapheme cluster boundary rules, as per
** http://www.unicode.org/reports/tr29/tr29-37.html
** including  GB9a and GB9b.
**
** unicode_grapheme_break_init() allocates an opaque
** unicode_grapheme_break_info_t handle, and
** unicode_grapheme_break_destroy() destroys it.
**
** Passing the handle to unicode_grapheme_break_next() returns non-0 if
** there's a grapheme break before the given character (and after the
** character that got passed in the previous call).
**
** The first call to unicode_grapheme_break_next() returns 1, as per GB1.
**
** unicode_grapheme_break() is a simplified interface that returns non-0
** if there is a grapheme break between the two characters. This simplified
** interface is equivalent to calling unicode_grapheme_break_init(),
** followed by two calls to unicode_grapheme_break_next(), and finally
** unicode_grapheme_break_deinit(), and returns the result of the second
** call to unicode_grapheme_break_next().
*/

struct unicode_grapheme_break_info_s;

typedef struct unicode_grapheme_break_info_s *unicode_grapheme_break_info_t;

extern unicode_grapheme_break_info_t unicode_grapheme_break_init();
extern int unicode_grapheme_break_next(unicode_grapheme_break_info_t, char32_t);
extern void unicode_grapheme_break_deinit(unicode_grapheme_break_info_t);

extern int unicode_grapheme_break(char32_t a, char32_t b);

typedef enum {

#include <courier-unicode-script-tab.h>

} unicode_script_t;

/*
** Look up the unicode script property, as per
** http://www.unicode.org/reports/tr24/tr24-31.html
*/

unicode_script_t unicode_script(char32_t a);

/*
** Implementation of line break rules, as per
** http://www.unicode.org/reports/tr14/tr14-45.html
**
** Invoke unicode_lb_init() to initialize the linebreaking algorithm. The
** first parameter is a callback function that gets invoked with two
** arguments: UNICODE_LB_{MANDATORY|NONE|ALLOWED}, and a passthrough argument.
** The second parameter to unicode_lb_init() is the opaque passthrough
** pointer, that is passed as the second argument to the callback function
** with no further interpretation.
**
** unicode_lb_init() returns an opaque handle. Invoke unicode_lb_next(),
** passing the handle and one unicode character. Repeatedly invoke
** unicode_lb_next() to specify the input string for the linebreaking
** algorithm, then invoke unicode_lb_end() to finish calculating the
** linebreaking algorithm, and deallocate the opaque linebreaking handle.
**
** The callback function gets invoked once for each invocation of
** unicode_lb_next(). The contract is that before unicode_lb_end() returns,
** the callback function will get invoked the exact number of times that
** unicode_lb_next(), as long as each invocation of the callback function
** returned 0; nothing more, nothing less. The first parameter to the callback
** function will be one of the following values:
**
** UNICODE_LB_MANDATORY - a linebreak is MANDATORY before the corresponding
** character.
** UNICODE_LB_NONE - a linebreak is PROHIBITED before the corresponding
** character.
** UNICODE_LB_ALLOWED - a linebreak is OPTIONAL before the corresponding
** character (the preceding character is a space, or an equivalent).
**
** The callback function should return 0. A non-zero value indicates an
** error, which gets propagated up to the caller. The contract that the
** callback function gets invoked the same number of times that
** unicode_lb_next() gets invoked is now broken.
*/

#define UNICODE_LB_MANDATORY	-1
#define UNICODE_LB_NONE		0
#define UNICODE_LB_ALLOWED	1

struct unicode_lb_info;

typedef struct unicode_lb_info *unicode_lb_info_t;

/*
** Allocate a linebreaking handle.
*/
extern unicode_lb_info_t unicode_lb_init(int (*cb_func)(int, void *),
					 void *cb_arg);

/*
** Feed the next character through the linebreaking algorithm.
** A non-zero return code indicates that the callback function was invoked
** and it returned a non-zero return code (which is propagated as a return
** value). unicode_lb_end() must still be invoked, in this case.
**
** A zero return code indicates that if the callback function was invoked,
** it returned 0.
*/

extern int unicode_lb_next(unicode_lb_info_t i, char32_t ch);

/*
** Convenience function that invokes unicode_lb_next() with a list of
** unicode chars. Returns 0 if all invocations of unicode_lb_next() returned
** 0, or the first non-zero return value from unicode_lb_next().
*/

extern int unicode_lb_next_cnt(unicode_lb_info_t i,
			       const char32_t *chars,
			       size_t cnt);

/*
** Finish the linebreaking algorithm.
**
** A non-zero return code indicates that the callback function was invoked
** and it returned a non-zero return code (which is propagated as a return
** value).
**
** A zero return code indicates that if the callback function was invoked,
** it returned 0, and that the callback function was invoked exactly the same
** number of times that unicode_lb_next() was invoked.
**
** In all case, the linebreak handle will no longer be valid when this
** function returns.
*/

extern int unicode_lb_end(unicode_lb_info_t i);

/*
** An alternative linebreak API where the callback function receives the
** original unicode character in addition to its linebreak value.
**
** User unicode_lbc_init(), unicode_lbc_next(), and unicode_lbc_end(), whose
** semantics are the same as their _lb_ counterparts.
*/

struct unicode_lbc_info;

typedef struct unicode_lbc_info *unicode_lbc_info_t;

extern unicode_lbc_info_t unicode_lbc_init(int (*cb_func)(int, char32_t,
							  void *),
					   void *cb_arg);
extern int unicode_lbc_next(unicode_lbc_info_t i, char32_t ch);
extern int unicode_lbc_next_cnt(unicode_lbc_info_t i,
				const char32_t *chars,
				size_t cnt);
extern int unicode_lbc_end(unicode_lbc_info_t i);

/*
** Set linebreaking options.
**
** OPTIONS SUBJECT TO CHANGE.
*/

extern void unicode_lb_set_opts(unicode_lb_info_t i, int opts);

extern void unicode_lbc_set_opts(unicode_lbc_info_t i, int opts);

/*
** Tailorization of LB24: Prevent pluses, as in "C++", from breaking.
**
** Adds the following to LB24:
**
**            PR x PR
**
**            AL x PR
**
**            ID x PR
**/
#define UNICODE_LB_OPT_PRBREAK 0x0001


/*
** Tailored "/" breaking rules. This prevents breaking after the "/"
** character. And provides an exception to the "x SY" rule in LB13.
**
** Adds the following rule to LB13:
**
**            SY x EX
**
**            SY x AL
**
**            SY x ID
**
**            SP ÷ SY, which takes precedence over "x SY".
*/
#define UNICODE_LB_OPT_SYBREAK 0x0002

/*
** Tailored / breaking rules.
**
** This reclassifies U+2013 and U+2014 as class WJ, prohibiting breaks before
** and after mdash and ndash.
*/
#define UNICODE_LB_OPT_DASHWJ 0x0004

/*
** Implemention of word break rules, as per
** http://www.unicode.org/reports/tr29/tr29-37.html
**
** Invoke unicode_wb_init() to initialize the wordbreaking algorithm. The
** first parameter is a callback function that gets invoked with two
** arguments: an int flag, and a passthrough argument. The second parameter to
** unicode_wb_init() is the opaque passthrough pointer, that is passed as the
** second argument to the callback function with no further interpretation.
**
** unicode_wb_init() returns an opaque handle. Invoke unicode_wb_next(),
** passing the handle and one unicode character. Repeatedly invoke
** unicode_wb_next() to specify the input string for the wordbreaking
** algorithm, then invoke unicode_wb_end() to finish calculating the
** wordbreaking algorithm, and deallocate the opaque wordbreaking handle.
**
** The callback function gets invoked once for each invocation of
** unicode_wb_next(). The contract is that before unicode_wb_end() returns,
** the callback function will get invoked the exact number of times that
** unicode_wb_next(), as long as each invocation of the callback function
** returned 0; nothing more, nothing less. The first parameter to the callback
** function will be an int. A non-zero value indicates that there is a word
** break between this character and the preceding one.
**
** The callback function should return 0. A non-zero value indicates an
** error, which gets propagated up to the caller. The contract that the
** callback function gets invoked the same number of times that
** unicode_lb_next() gets invoked is now broken.
*/

struct unicode_wb_info;

typedef struct unicode_wb_info *unicode_wb_info_t;

/*
** Allocate a wordbreaking handle.
*/
extern unicode_wb_info_t unicode_wb_init(int (*cb_func)(int, void *),
					 void *cb_arg);

/*
** Feed the next character through the wordbreaking algorithm.
** A non-zero return code indicates that the callback function was invoked
** and it returned a non-zero return code (which is propagated as a return
** value). unicode_wb_end() must still be invoked, in this case.
**
** A zero return code indicates that if the callback function was invoked,
** it returned 0.
*/

extern int unicode_wb_next(unicode_wb_info_t i, char32_t ch);

/*
** Convenience function that invokes unicode_wb_next() with a list of
** unicode chars. Returns 0 if all invocations of unicode_wb_next() returned
** 0, or the first non-zero return value from unicode_wb_next().
*/

extern int unicode_wb_next_cnt(unicode_wb_info_t i,
			       const char32_t *chars,
			       size_t cnt);

/*
** Finish the wordbreaking algorithm.
**
** A non-zero return code indicates that the callback function was invoked
** and it returned a non-zero return code (which is propagated as a return
** value).
**
** A zero return code indicates that if the callback function was invoked,
** it returned 0, and that the callback function was invoked exactly the same
** number of times that unicode_wb_next() was invoked.
**
** In all case, the wordbreak handle will no longer be valid when this
** function returns.
*/

extern int unicode_wb_end(unicode_wb_info_t i);

/*
** Search for a word boundary.
**
** Obtain a handle by calling unicode_wbscan_init(), then invoke
** unicode_wbscan_next() to provide a unicode stream, then invoke
** unicode_wbscan_end(). unicode_wbscan_end() returns the number of unicode
** characters from the beginning of the stream until the first word boundary.
**
** You may prematurely stop calling unicode_wbscan_next() once it returns a
** non-0 value, which means that there is sufficient context to compute the
** first word boundary, and all further calls to unicode_wbscan_next() will
** be internal no-ops.
*/

struct unicode_wbscan_info;

typedef struct unicode_wbscan_info *unicode_wbscan_info_t;

unicode_wbscan_info_t unicode_wbscan_init();

int unicode_wbscan_next(unicode_wbscan_info_t i, char32_t ch);

size_t unicode_wbscan_end(unicode_wbscan_info_t i);

/* Unicode directional markers */

#define UNICODE_LRM	0x200E /* Left-to-right marker */
#define UNICODE_RLM	0x200F /* Right-to-left marker */
#define UNICODE_ALM	0x061C /* Right-to-left Arabic marker */
#define UNICODE_LRI	0x2066 /* Left-to-right isolate */
#define UNICODE_RLI	0x2067 /* Right-to-left isolate */
#define UNICODE_PDI	0x2069 /* Pop isolate */
#define UNICODE_RLO	0x202e /* Right-to-left override */
#define UNICODE_LRO	0x202d /* Left-to-right override */
#define UNICODE_PDF	0x202c /* Pop directional override */

#ifdef __cplusplus
#if __cplusplus >= 201103L
namespace unicode {
	namespace literals {

		constexpr char32_t LRM[]={UNICODE_LRM, 0};
		constexpr char32_t RLM[]={UNICODE_RLM, 0};
		constexpr char32_t ALM[]={UNICODE_ALM, 0};
		constexpr char32_t LRI[]={UNICODE_LRI, 0};
		constexpr char32_t RLI[]={UNICODE_RLI, 0};
		constexpr char32_t PDI[]={UNICODE_PDI, 0};
		constexpr char32_t RLO[]={UNICODE_RLO, 0};
		constexpr char32_t LRO[]={UNICODE_LRO, 0};
		constexpr char32_t PDF[]={UNICODE_PDF, 0};
	}
}
#endif
#endif

typedef char unicode_bidi_bracket_type_t;

#define UNICODE_BIDI_n  'n'
#define UNICODE_BIDI_o	'o'
#define UNICODE_BIDI_c	'c'

extern char32_t unicode_bidi_mirror(char32_t c);

extern char32_t unicode_bidi_bracket_type(char32_t c,
					  unicode_bidi_bracket_type_t *ret);


typedef unsigned char unicode_bidi_level_t;

#define UNICODE_BIDI_LR		((unicode_bidi_level_t)0)
#define UNICODE_BIDI_RL		((unicode_bidi_level_t)1)
#define UNICODE_BIDI_SKIP	((unicode_bidi_level_t)254)

/*
** What unicode_bidi_direction returns.
*/

struct unicode_bidi_direction {

	/* Direction of the given text */
	unicode_bidi_level_t direction;

	/*
	** The direction is explicit, if not direction is UNICODE_BIDI_LR by
	** default.
	*/
	int is_explicit;
};

struct unicode_bidi_direction unicode_bidi_get_direction(const char32_t *p,
							 size_t n);

struct unicode_bidi_direction unicode_bidi_calc(const char32_t *p, size_t n,
						unicode_bidi_level_t *bufp,
						const unicode_bidi_level_t *
						initial_embedding_level);

extern void unicode_bidi_reorder(char32_t *p,
				 unicode_bidi_level_t *levels,
				 size_t n,
				 void (*reorder_callback)(size_t, size_t,
							  void *),
				 void *arg);

/* BIDI_TYPE_LIST */
typedef enum {
	      UNICODE_BIDI_TYPE_AL,
	      UNICODE_BIDI_TYPE_AN,
	      UNICODE_BIDI_TYPE_B,
	      UNICODE_BIDI_TYPE_BN,
	      UNICODE_BIDI_TYPE_CS,
	      UNICODE_BIDI_TYPE_EN,
	      UNICODE_BIDI_TYPE_ES,
	      UNICODE_BIDI_TYPE_ET,
	      UNICODE_BIDI_TYPE_FSI,
	      UNICODE_BIDI_TYPE_L,
	      UNICODE_BIDI_TYPE_LRE,
	      UNICODE_BIDI_TYPE_LRI,
	      UNICODE_BIDI_TYPE_LRO,
	      UNICODE_BIDI_TYPE_NSM,
	      UNICODE_BIDI_TYPE_ON,
	      UNICODE_BIDI_TYPE_PDF,
	      UNICODE_BIDI_TYPE_PDI,
	      UNICODE_BIDI_TYPE_R,
	      UNICODE_BIDI_TYPE_RLE,
	      UNICODE_BIDI_TYPE_RLI,
	      UNICODE_BIDI_TYPE_RLO,
	      UNICODE_BIDI_TYPE_S,
	      UNICODE_BIDI_TYPE_WS,
} enum_bidi_type_t;

extern enum_bidi_type_t unicode_bidi_type(char32_t c);

extern void unicode_bidi_calc_types(const char32_t *p, size_t n,
				    enum_bidi_type_t *buf);

extern void unicode_bidi_setbnl(char32_t *p,
				const enum_bidi_type_t *types,
				size_t n);

extern struct unicode_bidi_direction
unicode_bidi_calc_levels(const char32_t *p,
			 const enum_bidi_type_t
			 *types,
			 size_t n,
			 unicode_bidi_level_t *bufp,
			 const unicode_bidi_level_t
			 *initial_embedding_level);

/* Bitmask options to unicode_bidi_cleanup */

/*
 In addition to removing embedding, override, and boundary-neutral
 characters also remove isolation markers and implicit markers.
*/

#define UNICODE_BIDI_CLEANUP_EXTRA	1

/*
  Replace all characters classified as paragraph separators by a newline
  character.
*/

#define UNICODE_BIDI_CLEANUP_BNL	2

/*
  Options for canonical rendering order.
*/

#define UNICODE_BIDI_CLEANUP_CANONICAL				\
	(UNICODE_BIDI_CLEANUP_EXTRA | UNICODE_BIDI_CLEANUP_BNL)

#ifdef __cplusplus
#if __cplusplus >= 201103L
namespace unicode {
	namespace literals {
		constexpr int CLEANUP_EXTRA=UNICODE_BIDI_CLEANUP_EXTRA;

		constexpr int CLEANUP_BNL=UNICODE_BIDI_CLEANUP_BNL;

		constexpr int CLEANUP_CANONICAL=UNICODE_BIDI_CLEANUP_CANONICAL;
	}
}
#endif
#endif

extern size_t unicode_bidi_cleanup(char32_t *string,
				   unicode_bidi_level_t *levels,
				   size_t n,
				   int options,
				   void (*removed_callback)(size_t, void *),
				   void *);

extern size_t unicode_bidi_cleaned_size(const char32_t *string,
					size_t n,
					int options);

extern void unicode_bidi_logical_order(char32_t *string,
				       unicode_bidi_level_t *levels,
				       size_t n,
				       unicode_bidi_level_t paragraph_embedding,
				       void (*reorder_callback)(size_t, size_t,
								void *),
				       void *arg);

extern int unicode_bidi_needs_embed(const char32_t *string,
				    const unicode_bidi_level_t *levels,
				    size_t n,
				    const unicode_bidi_level_t *
				    paragraph_embedding);

extern void unicode_bidi_embed(const char32_t *string,
			       const unicode_bidi_level_t *levels,
			       size_t n,
			       unicode_bidi_level_t paragraph_embedding,
			       void (*emit)(const char32_t *string,
					    size_t n,
					    int,
					    void *arg),
			       void *arg);

extern char32_t unicode_bidi_embed_paragraph_level(const char32_t *str,
						   size_t n,
						   unicode_bidi_level_t);

extern void unicode_bidi_combinings(const char32_t *str,
				    const unicode_bidi_level_t *levels,
				    size_t n,
				    void (*combinings)
				    (unicode_bidi_level_t level,
				     size_t level_start,
				     size_t n_chars,
				     size_t comb_start,
				     size_t n_comb_chars,
				     void *arg),
				    void *arg);

/*
** unicode_canonical() returns the canonical mapping of the given Unicode
** character. The returned structure specifies:
**
** - A pointer to the canonical decomposition of the given character.
** - Number of characters in the canonical decomposition.
** - An optional formatting tag.
**
** A null pointer, and a 0 character count gets returned for characters
** without a canonical decomposition.
**
*/

typedef enum {
	      UNICODE_CANONICAL_FMT_NONE=0,

	      UNICODE_CANONICAL_FMT_CIRCLE,
	      UNICODE_CANONICAL_FMT_COMPAT,
	      UNICODE_CANONICAL_FMT_FINAL,
	      UNICODE_CANONICAL_FMT_FONT,
	      UNICODE_CANONICAL_FMT_FRACTION,
	      UNICODE_CANONICAL_FMT_INITIAL,
	      UNICODE_CANONICAL_FMT_ISOLATED,
	      UNICODE_CANONICAL_FMT_MEDIAL,
	      UNICODE_CANONICAL_FMT_NARROW,
	      UNICODE_CANONICAL_FMT_NOBREAK,
	      UNICODE_CANONICAL_FMT_SMALL,
	      UNICODE_CANONICAL_FMT_SQUARE,
	      UNICODE_CANONICAL_FMT_SUB,
	      UNICODE_CANONICAL_FMT_SUPER,
	      UNICODE_CANONICAL_FMT_VERTICAL,
	      UNICODE_CANONICAL_FMT_WIDE,
} unicode_canonical_fmt_t;

typedef struct {
	const char32_t *canonical_chars;
	size_t n_canonical_chars;
	unicode_canonical_fmt_t format;
} unicode_canonical_t;

extern unicode_canonical_t unicode_canonical(char32_t);

/*
** A buffer that holds unicode characters, and dynamically grows as needed.
*/

struct unicode_buf {
	char32_t *ptr;	/* The unicode characters */
	size_t size,		/* Buffer size */
		len,		/* How many characters in ptr are initialized */
		max;		/* Maximum size the buffer can grow to */
};

/*
** Initialize a buffer. Constructor.
*/

void unicode_buf_init(/* Initialize this structure. ptr, size, len cleared */
		      struct unicode_buf *p,

		      /*
		      ** Maximum size the buffer can grow to. (size_t)-1
		      ** means unlimited.
		      */
		      size_t max);
/*
** Like unicode_buf_init, and initialize the new buffer with the contents of
** another buffer. The maximum size of the initialized buffer is exactly the
** number of characters in the existing buffer. This copies a buffer using
** the minimum amount of heap space.
*/

#define unicode_buf_init_copy(a,b)				\
	do {							\
		unicode_buf_init((a), unicode_buf_len(b));	\
		unicode_buf_append_buf((a),(b));		\
	} while (0)

/*
** Deinitialize the buffer. Destructor. Frees memory.
*/

void unicode_buf_deinit(struct unicode_buf *p);

/*
** Official way to access the characters in the unicode buffer.
*/
#define unicode_buf_ptr(p) ((p)->ptr)

/*
** Official way of obtaining the number of characters in the unicode buffer.
*/
#define unicode_buf_len(p) ((p)->len)

/*
** Remove all existing characters from an initialized buffer. Sets len to 0.
*/

#define unicode_buf_clear(p) ((p)->len=0)

/*
** Append characters to the existing characters in the unicode buffer.
** The buffer grows, if needed. If the buffer would exceed its maximum size,
** the extra characters get truncated.
**
** Returns 0 if the characters were appended. -1 for a malloc failure.
*/

int unicode_buf_append(struct unicode_buf *p,	/* The buffer */
		       const char32_t *uc,	/* Characters to append */
		       size_t l);		/* How many of them */

/*
** Convert an iso-8859-1 char string and invoke unicode_buf_append().
*/

void unicode_buf_append_char(struct unicode_buf *dst,
			     const char *str,
			     size_t cnt);

/*
** Remove some portion of the unicode buffer
*/

void unicode_buf_remove(struct unicode_buf *p, /* The buffer */
			size_t pos, /* Offset in buffer */
			size_t cnt); /* How many to remove */

/*
** Append the contents of an existing buffer to another one.
*/

#define unicode_buf_append_buf(a,b)					\
	unicode_buf_append((a), unicode_buf_ptr(b), unicode_buf_len(b))


/*
** The equivalent of strcmp() for unicode buffers.
*/

int unicode_buf_cmp(const struct unicode_buf *a,
		    const struct unicode_buf *b);

/*
** The equivalent of unicode_buf_cmp, except that the second buffer is an
** iso-8859-1 string.
*/

int unicode_buf_cmp_str(const struct unicode_buf *p,
			const char *c,	/* iso-8859-1 string */
			size_t cl);	/* Number of chars in c */

/*
** A wrapper for iconv(3). This wrapper provides a different API for iconv(3).
** A handle gets created by unicode_convert_init().
** unicode_convert_init() receives a pointer to the output function
** which receives converted character text.
**
** The output function receives a pointer to the converted character text, and
** the number of characters in the converted text.
**
** The character text to convert gets passed, repeatedly, to
** unicode_convert(). Each call to unicode_convert() results in
** the output function being invoked, zero or more times, with the converted
** text. Finally, unicode_convert_deinit() stops the conversion and
** deallocates the conversion handle.
**
** Internal buffering takes place. unicode_convert_deinit() may result
** in the output function being called one or more times, to receive the final
** part of the converted character stream.
**
** The output function should return 0. A non-0 value causes
** unicode_convert() and/or unicode_convert_deinit() returning
** non-0.
*/

struct unicode_convert_hdr;

typedef struct unicode_convert_hdr *unicode_convert_handle_t;

/*
** unicode_convert_init() returns a non-NULL handle for the requested
** conversion, or NULL if the requested conversion is not available.
*/

unicode_convert_handle_t
unicode_convert_init(/* Convert from this chset */
		       const char *src_chset,

		       /* Convert to this chset */
		       const char *dst_chset,

		       /* The output function */

		       int (*output_func)(const char *, size_t, void *),

		       /* Passthrough arg */
		       void *convert_arg);

/*
** Repeatedly pass the character text to convert to unicode_convert().
**
** Returns non-0 if the output function returned non-0, or 0 if all invocations
** of the output function returned 0.
*/

int unicode_convert(/* The conversion handle */
		      unicode_convert_handle_t handle,

		      /* Text to convert */
		      const char *text,

		      /* Number of bytes to convert */
		      size_t cnt);

/*
** Finish character set conversion. The handle gets deallocated.
**
** May still result in one or more invocations of the output function.
** Returns non-zero if any previous invocation of the output function returned
** non-zero (this includes any invocations of the output function resulting
** from this call, or prior unicode_convert() calls), or 0 if all
** invocations of the output function returned 0.
**
** If the errptr is not NULL, *errptr is set to non-zero if there were any
** conversion errors -- if there was any text that could not be converted to
** the destination character text.
*/

int unicode_convert_deinit(unicode_convert_handle_t handle,
			     int *errptr);


/*
** Specialization: save converted character text in a buffer.
**
** Implementation: call unicode_convert_tocbuf_init() instead of
** unicode_convert_init(), then call unicode_convert() and
** unicode_convert_deinit(), as usual.
**
** If unicode_convert_deinit() returns 0, *cbufptr_ret gets initialized to a
** malloc()ed buffer, and the number of converted characters, the size of the
** malloc()ed buffer, are placed into *csize_ret arguments, that were passed
** to unicode_convert_tou_init().
**
** Note: if the converted string is an empty string, *cbufsize_ret is set to 0,
** but *cbufptr_ptr still gets initialized (to a dummy malloced buffer).
**
** The optional nullterminate places a trailing \0 character after the
** converted string (this is included in *cbufsize_ret).
*/

unicode_convert_handle_t
unicode_convert_tocbuf_init(/* Convert from this chset */
			      const char *src_chset,

			      /* Convert to this chset */
			      const char *dst_chset,

			      /* malloced buffer */
			      char **cbufptr_ret,

			      /* size of the malloced buffer */
			      size_t *cbufsize_ret,

			      /* null terminate the resulting string */
			      int nullterminate
			      );


/*
** Specialization: convert some character text to a char32_t array.
**
** This is like unicode_convert_tocbuf_init(), but converts to a char32_t
** array.
**
** The returned *ucsize_ret is initialized with the number of char32_ts,
** rather than the byte count.
**
** In all other ways, this function behaves identically to
** unicode_convert_tocbuf_init().
*/

unicode_convert_handle_t
unicode_convert_tou_init(/* Convert from this chset */
			   const char *src_chset,

			   /* malloc()ed buffer pointer, on exit. */
			   char32_t **ucptr_ret,

			   /* size of the malloc()ed buffer, upon exit */
			   size_t *ucsize_ret,

			   /* If true, terminate with U+0x0000, for convenience */
			   int nullterminate
			   );

/*
** Specialization: convert a char32_t array to some character text.
**
** This is the opposite of unicode_convert_tou_init(). Call this to
** initialize the conversion handle, then use unicode_convert_uc()
** instead of unicode_convert.
*/

unicode_convert_handle_t
unicode_convert_fromu_init(/* Convert to this chset */
			     const char *dst_chset,

			     /* malloc()ed buffer pointer, on exit. */
			     char **cbufptr_ret,

			     /* size of the malloc()ed buffer, upon exit */
			     size_t *cbufsize_ret,

			     /* If true, terminate with U+0x0000, for convenience */
			     int nullterminate
			     );

int unicode_convert_uc(/* The conversion handle */
			 unicode_convert_handle_t handle,

			 /* Text to convert */
			 const char32_t *text,

			 /* Number of bytes to convert */
			 size_t cnt);

/*
** Initialize conversion to UTF-8.
**
** This is a wrapper for unicode_convert_tocbuf_init() that specifies the
** destination charset as UTF-8.
*/

unicode_convert_handle_t
unicode_convert_tocbuf_toutf8_init(const char *src_chset,
				     char **cbufptr_ret,
				     size_t *cbufsize_ret,
				     int nullterminate);

/*
** Initialize conversion from UTF-8.
**
** This is a wrapper for unicode_convert_tocbuf_init() that specifies the
** source charset as UTF-8.
*/

unicode_convert_handle_t
unicode_convert_tocbuf_fromutf8_init(const char *dst_chset,
				       char **cbufptr_ret,
				       size_t *cbufsize_ret,
				       int nullterminate);

/*
** Convert a character string to UTF-8.
**
** Returns a malloc-ed buffer holding the UTF-8 string, or NULL if an
** error occured.
*/
char *unicode_convert_toutf8(/* Text to convert to UTF-8 */
			       const char *text,

			       /* Character set to convert to UTF-8 */
			       const char *charset,

			       /*
			       ** If non-NULL, and a non-NULL pointer is
			       ** returned, *error is set to non-zero if
			       ** a character conversion error has occured.
			       */
			       int *error);

/*
** Convert UTF-8 text to another character set.
**
** Returns a malloc-ed buffer holding the string converted to the specified
** character set, or NULL if an error occured.
*/

char *unicode_convert_fromutf8(/* A UTF-8 string */
				 const char *text,

				 /*
				 ** Convert the UTF-8 string to this character
				 ** set.
				 */

				 const char *charset,

				 /*
				 ** If non-NULL, and a non-NULL pointer is
				 ** returned, *error is set to non-zero if
				 ** a character conversion error has occured.
				 */
				 int *error);

/*
** Convert one charset to another charset, placing the result in a malloc-ed
** buffer.
**
** Returns a malloc-ed buffer holding the string converted to the specified
** character set, or NULL if an error occured.
*/

char *unicode_convert_tobuf(/* A string to convert */
			      const char *text,

			      /*
			      ** String's charset.
			      */

			      const char *charset,

			      /*
			      ** Destination charset
			      */
			      const char *dstcharset,

			      /*
			      ** If non-NULL, and a non-NULL pointer is
			      ** returned, *error is set to non-zero if
			      ** a character conversion error has occured.
			      */
			      int *error);

/*
** Convenience function: call unicode_convert_tou_init(), feed the
** character string through unicode_convert(), then call
** unicode_convert_deinit().
**
** If this function returns 0, *uc and *ucsize is set to a malloced buffer+size
** holding the unicode char array.
*/

int unicode_convert_tou_tobuf(/* Character text to convert */
				const char *text,

				/* Number of characters */
				size_t text_l,

				/* text's charset */
				const char *charset,

				/*
				** If this function returns 0, this gets
				** initialized
				*/
				char32_t **uc,

				/*
				** Size of the allocated buffer
				*/
				size_t *ucsize,

				/*
				** If not null and this function returns 0,
				** this is set to non-0 if there
				** was a conversion error (but the output
				** buffer gets still allocated and
				** initialized)
				*/
				int *err);

/*
** Convenience function: call unicode_convert_fromu_init(), feed the
** unicode_array through unicode_convert_uc(), then call
** unicode_convert_deinit().
**
** If this function returns 0, *uc and *ucsize is set to a malloced buffer+size
** holding the converted character string
*/

int unicode_convert_fromu_tobuf(/* Unicode array to convert to a char str */
				  const char32_t *utext,

				  /*
				  ** Size of the unicode array.
				  ** If this is (size_t)-1, utext is a
				  ** 0-terminated array.
				  */
				  size_t utext_l,

				  /*
				  ** Convert the unicode array to this charset.
				  */
				  const char *charset,

				  /*
				  ** If unicode_convert_fromu_tobuf()
				  ** returns 0, this is initialized to a
				  ** malloced buffer with a 0-terminated
				  ** string is kept.
				  */
				  char **c,

				  /*
				  ** Size of the initialized array, including
				  ** the 0-terminator.
				  */
				  size_t *csize,

				  /*
				  ** If unicode_convert_fromu_tobuf()
				  ** returns 0 and this is not NULL,
				  ** *err is set to non-0 if there was a
				  ** conversion error to the requested
				  ** character set.
				  */
				  int *err);

/*
** Convenience function: convert a string in a given character set
** to/from uppercase, lowercase, or something else.
**
** This is done by calling unicode_convert_tou_tobuf() first,
** applying the title_func and char_func, then using
** unicode_convert_fromu_tobuf().
**
** A NULL return indicates that the requested conversion cannot be performed.
*/

char *unicode_convert_tocase( /* String to convert */
			       const char *str,

			       /* String's character set */

			       const char *charset,

			       /*
			       ** Conversion of the first character in
			       ** str: unicode_uc, unicode_lc, or unicode_tc:
			       */

			       char32_t (*first_char_func)(char32_t),

			       /*
			       ** Conversion of the second and the remaining
			       ** character in str. If NULL, same as
			       ** first_char_func.
			       */
			       char32_t (*char_func)(char32_t));



/* Either UCS-4BE or UCS-4LE, matching the native char32_t endianness */

extern const char unicode_u_ucs4_native[];

/* Either UCS-2BE or UCS-2LE, matching the native char32_t endianness */

extern const char unicode_u_ucs2_native[];

/*
** Modified-UTF7 encoding used for IMAP folder names. Pass it for a charset
** parameter.
**
** This can be followed by a " " and up to 15 characters to be escaped in
** addition to unicode chars.
*/

#define unicode_x_imap_modutf7 "x-imap-modutf7"

/*
** EAI-capable Courier-IMAP does not use modified-UTF7, and uses UTF-8.
**
** However, to support SMAP we will still need to encode/decode some
** special characters.
**
** The characters U+0000-U+001F, and ./~:\
**
** They are encoded as a backslash followed by three octal digits.
*/

#define unicode_x_smap_modutf8 "x-smap-modutf8"

extern uint8_t unicode_ccc(char32_t ch);

#define UNICODE_DECOMPOSE_FLAG_QC	1
#define UNICODE_DECOMPOSE_FLAG_COMPAT	2

typedef struct unicode_decomposition {
	char32_t *string;
	size_t string_size;
	int decompose_flags;

	int (*reallocate)(struct unicode_decomposition *info,
			  const size_t *offsets,
			  const size_t *sizes,
			  size_t n);

	void *arg;
} unicode_decomposition_t;

extern void unicode_decomposition_init(unicode_decomposition_t *,
				       char32_t *string,
				       size_t string_size,
				       void *arg);

extern void unicode_decomposition_deinit(unicode_decomposition_t *);

extern int unicode_decompose(unicode_decomposition_t *);

extern size_t unicode_decompose_reallocate_size(unicode_decomposition_t *,
						const size_t *sizes,
						size_t n);

struct unicode_compose_info {
	size_t index;
	size_t n_composed;
	char32_t *composition;
	size_t n_composition;
};

typedef struct {
	struct unicode_compose_info **compositions;
	size_t n_compositions;
} unicode_composition_t;

#define UNICODE_COMPOSE_FLAG_REMOVEUNUSED 1
#define UNICODE_COMPOSE_FLAG_ONESHOT 128

int unicode_composition_init(const char32_t *string,
			     size_t string_size,
			     int flags,
			     unicode_composition_t *info);

void unicode_composition_deinit(unicode_composition_t *info);

size_t unicode_composition_apply(char32_t *string,
				 size_t string_size,
				 unicode_composition_t *info);

int unicode_compose(char32_t *string,
		    size_t string_size,
		    int flags,
		    size_t *new_size);

#if 0
{
#endif

#ifdef	__cplusplus
}

extern size_t unicode_wcwidth(const std::u32string &uc);

namespace unicode {

#if 0
};
#endif

/*
** Various character sets
*/
extern const char ucs_4[], ucs_2[], utf_8[], iso_8859_1[];

/*
** Interface to iconv.
**
** Subclass converted(). Invoke begin(), then operator(), repeatedly,
** then end().
**
** converted() receives the converted text.
*/

class iconvert {

	unicode_convert_handle_t handle;

 public:
	iconvert();
	virtual ~iconvert();

	/* Start conversion.
	** Returns false if the requested conversion cannot be done.
	**/

	bool begin(/* Convert from */
		   const std::string &src_chset,

		   /* Convert to */
		   const std::string &dst_chset);

	/* Feed iconv(3). Returns false if the conversion was aborted.
	 */

	bool operator()(const char *, size_t);

	bool operator()(const char32_t *, size_t);

	/*
	** Get the results here. If the subclass returns a non-0
	** value, the conversion is aborted.
	*/

	virtual int converted(const char *, size_t);

	/*
	** End of conversion.
	**
	** Returns true if all calls to converted() returned 0,
	** false if the conversion was aborted.
	**
	** errflag is set to true if there was a character that could
	** not be converted, and passed to converted().
	*/

	bool end(bool &errflag)
	{
		return end(&errflag);
	}

	bool end()
	{
		return end(NULL);
	}

	/* Convert between two different charsets */

	static std::string convert(const std::string &text,
				   const std::string &charset,
				   const std::string &dstcharset,
				   bool &errflag);

	/* Convert between two different charsets */

	static std::string convert(const std::string &text,
				   const std::string &charset,
				   const std::string &dstcharset)
	{
		bool dummy;

		return convert(text, charset, dstcharset, dummy);
	}

	/* Convert from unicode to a charset */

	static std::string convert(const std::u32string &uc,
				   const std::string &dstcharset,
				   bool &errflag);

	/* Convert from unicode to a charset */

	static std::string convert(const std::u32string &uc,
				   const std::string &dstcharset)
	{
		bool dummy;

		return convert(uc, dstcharset, dummy);
	}

	/* Convert charset to unicode */

	static bool convert(const std::string &text,
			    const std::string &charset,
			    std::u32string &uc);


	/* Convert to upper/lower/title case */

	static std::string
		convert_tocase(/* Text string */
			       const std::string &text,

			       /* Its charset */
			       const std::string &charset,

			       /* First character: unicode_uc, unicode_lc, or unicode_tc */
			       char32_t (*first_char_func)(char32_t),

			       /* If not NULL, second and subsequent chars */
			       char32_t (*char_func)(char32_t)
			       =NULL)
	{
		bool dummy;

		return convert_tocase(text, charset, dummy,
				      first_char_func,
				      char_func);
	}

	/* Convert to upper/lower/title case */

	static std::string
		convert_tocase(/* Text string */
			       const std::string &text,

			       /* Its charset */
			       const std::string &charset,

			       /* Set if there's a conversion error */
			       bool &err,

			       /* First character: unicode_uc, unicode_lc, or unicode_tc */
			       char32_t (*first_char_func)(char32_t),

			       /* If not NULL, second and subsequent chars */
			       char32_t (*char_func)(char32_t)
			       =NULL);
 private:
	bool end(bool *);

 public:
	class tou;
	class fromu;
};

/* Convert output of iconvert to char32_ts. */

class iconvert::tou : public iconvert {

 public:
	bool begin(const std::string &chset);

	virtual int converted(const char32_t *, size_t);

	using iconvert::operator();
 private:
	int converted(const char *ptr, size_t cnt);

 public:
	template<typename iter_t> class to_iter_class;

	template<typename input_iter_t,
		typename output_iter_t>
		static output_iter_t convert(input_iter_t from_iter,
					     input_iter_t to_iter,
					     const std::string &chset,
					     bool &flag,
					     output_iter_t out_iter);

	template<typename input_iter_t>
		static bool convert(input_iter_t from_iter,
				    input_iter_t to_iter,
				    const std::string &chset,
				    std::u32string &out_buf)
	{
		bool flag;

		out_buf.clear();
		std::back_insert_iterator<std::u32string >
			insert_iter(out_buf);

		convert(from_iter, to_iter, chset, flag, insert_iter);

		return flag;
	}

	static std::pair<std::u32string, bool>
		convert(const std::string &str,
			const std::string &chset);
};

/* Helper class that saves unicode output into an output iterator */

template<typename iter_t>
class iconvert::tou::to_iter_class : public iconvert::tou {

	iter_t iter;
 public:

 to_iter_class(iter_t iterValue)
	 : iter(iterValue) {}

	using tou::operator();

	operator iter_t() const { return iter; }

 private:
	int converted(const char32_t *ptr, size_t cnt)
	{
		while (cnt)
		{
			*iter=*ptr;

			++iter;
			++ptr;
			--cnt;
		}
		return 0;
	}
};

template<typename input_iter_t,
	typename output_iter_t>
	output_iter_t iconvert::tou::convert(input_iter_t from_iter,
					     input_iter_t to_iter,
					     const std::string &chset,
					     bool &flag,
					     output_iter_t out_iter)
{
	class to_iter_class<output_iter_t> out(out_iter);

	if (!out.begin(chset))
		return out;

	std::vector<char> string;

	while (from_iter != to_iter)
	{
		string.push_back(*from_iter++);

		if (string.size() > 31)
		{
			out(&string[0], string.size());
			string.clear();
		}
	}

	if (string.size() > 0)
		out(&string[0], string.size());

	out.end(flag);
	return out;
}

/* Convert output of iconvert from char32_ts. */

class iconvert::fromu : public iconvert {

 public:
	bool begin(const std::string &chset);

	using iconvert::operator();

	template<typename iter_t> class to_iter_class;

	template<typename input_iter_t,
		typename output_iter_t>
		static output_iter_t convert(input_iter_t from_iter,
					     input_iter_t to_iter,
					     const std::string &chset,
					     output_iter_t out_iter,
					     bool &errflag);

	template<typename input_iter_t>
		static void convert(input_iter_t from_iter,
				    input_iter_t to_iter,
				    const std::string &chset,
				    std::string &out_buf,
				    bool &errflag)
	{
		out_buf="";
		std::back_insert_iterator<std::string>
			insert_iter(out_buf);

		convert(from_iter, to_iter, chset, insert_iter,
			errflag);
	}

	static std::pair<std::string, bool>
		convert(const std::u32string &ubuf,
			const std::string &chset);
};

/* Helper class that saves unicode output into an output iterator */

template<typename iter_t>
class iconvert::fromu::to_iter_class : public iconvert::fromu {

	iter_t iter;
 public:

 to_iter_class(iter_t iterValue)
	 : iter(iterValue) {}

	using fromu::operator();

	operator iter_t() const { return iter; }

 private:
	int converted(const char *ptr, size_t cnt)
	{
		while (cnt)
		{
			*iter=*ptr;

			++iter;
			++ptr;
			--cnt;
		}
		return 0;
	}
};

template<typename input_iter_t,
	typename output_iter_t>
	output_iter_t iconvert::fromu::convert(input_iter_t from_iter,
					       input_iter_t to_iter,
					       const std::string &chset,
					       output_iter_t out_iter,
					       bool &errflag)
{
	errflag=true;

	class to_iter_class<output_iter_t> out(out_iter);

	if (!out.begin(chset))
		return out;

	std::u32string string;

	while (from_iter != to_iter)
	{
		string.push_back(*from_iter++);

		if (string.size() > 31)
		{
			out(&string[0], string.size());
			string.clear();
		}
	}

	if (string.size() > 0)
		out(&string[0], string.size());

	out.end(errflag);
	return out;
}

/*
** Unicode linebreaking algorithm, tr14.
*/

extern "C" int linebreak_trampoline(int value, void *ptr);
extern "C" int linebreakc_trampoline(int value, char32_t ch,
				     void *ptr);

/*
** Subclass linebreak_callback_base, implement operator()(int).
**
** Use operator<< or operator()(iterator, iterator) to feed
** char32_ts into the linebreaking algorithm. The subclass receives
** UNICODE_LB values, as they become available.
*/

class linebreak_callback_base {

	unicode_lb_info_t handle;

	int opts;

#if __cplusplus >= 201103L
public:
	linebreak_callback_base(const linebreak_callback_base &)=delete;
	linebreak_callback_base &operator=(const
					   linebreak_callback_base &)=delete;
private:
#else
	linebreak_callback_base(const linebreak_callback_base &);
	/* NOT IMPLEMENTED */

	linebreak_callback_base &operator=(const
					   linebreak_callback_base &);
	/* NOT IMPLEMENTED */
#endif
public:
	linebreak_callback_base();
	virtual ~linebreak_callback_base();

	void finish();

	void set_opts(int opts);

	friend int linebreak_trampoline(int, void *);

	linebreak_callback_base &operator<<(char32_t uc);

	template<typename iter_type>
	linebreak_callback_base &operator()(iter_type beg_iter,
					    iter_type end_iter)
	{
		while (beg_iter != end_iter)
			operator<<(*beg_iter++);
		return *this;
	}

	template<typename container_type>
	linebreak_callback_base &operator()(const container_type &vec)
	{
		return operator()(vec.begin(), vec.end());
	}
private:
	virtual int callback(int);
};

class linebreak_callback_save_buf : public linebreak_callback_base {

public:
	std::list<int> lb_buf;

	linebreak_callback_save_buf();
	~linebreak_callback_save_buf();

	using linebreak_callback_base::operator<<;
	using linebreak_callback_base::operator();
private:
	int callback(int value);
};

/*
** Convert an input iterator sequence over char32_ts into
** an input iterator sequence over linebreak values.
*/

template<typename input_t> class linebreak_iter
	: public std::iterator<std::input_iterator_tag, int, void>
{
	mutable input_t iter_value, end_iter_value;

	mutable linebreak_callback_save_buf *buf;

	void fill() const
	{
		if (buf == NULL)
			return;

		while (buf->lb_buf.empty())
		{
			if (iter_value == end_iter_value)
			{
				buf->finish();
				if (buf->lb_buf.empty())
				{
					delete buf;
					buf=NULL;
				}
				break;
			}

			buf->operator<<(*iter_value++);
		}
	}

	mutable value_type bufvalue;

public:
	linebreak_iter(const input_t &iter_valueArg,
		       const input_t &iter_endvalueArg)
		: iter_value(iter_valueArg),
		  end_iter_value(iter_endvalueArg),
		  buf(new linebreak_callback_save_buf)
	{
	}

	linebreak_iter() : buf(NULL)
	{
	}

	void set_opts(int opts)
	{
		if (buf)
			buf->set_opts(opts);
	}

	~linebreak_iter()
	{
		if (buf)
			delete buf;
	}

	linebreak_iter(const linebreak_iter<input_t> &v)
		: buf(NULL)
	{
		operator=(v);
	}

	linebreak_iter<input_t> &operator=(const
					   linebreak_iter<input_t> &v)
	{
		if (buf)
			delete buf;
		buf=v.buf;
		iter_value=v.iter_value;
		end_iter_value=v.end_iter_value;
		v.buf=NULL;
		return *this;
	}

	bool operator==(const linebreak_iter<input_t> &v) const
	{
		fill();
		v.fill();

		return buf == NULL && v.buf == NULL;
	}

	bool operator!=(const linebreak_iter<input_t> &v) const
	{
		return !operator==(v);
	}

	value_type operator*() const
	{
		fill();
		return buf == NULL ? UNICODE_LB_MANDATORY:
			buf->lb_buf.front();
	}

	linebreak_iter<input_t> &operator++()
	{
		bufvalue=operator*();

		if (buf)
			buf->lb_buf.pop_front();
		return *this;
	}

	const value_type *operator++(int)
	{
		operator++();
		return &bufvalue;
	}
};

/*
** Like linebreak_callback_base, except the subclass receives both
** the linebreaking value, and the unicode character.
*/

class linebreakc_callback_base {

	unicode_lbc_info_t handle;

	int opts;

#if __cplusplus >= 201103L
public:
	linebreakc_callback_base(const linebreakc_callback_base &)
	=delete;

	linebreakc_callback_base &operator=(const
					    linebreakc_callback_base
					    &)=delete;
private:
#else
	linebreakc_callback_base(const linebreakc_callback_base &);
	/* NOT IMPLEMENTED */

	linebreakc_callback_base &operator=(const
					    linebreakc_callback_base
					    &);
	/* NOT IMPLEMENTED */
#endif

public:
	linebreakc_callback_base();
	virtual ~linebreakc_callback_base();

	void finish();

	void set_opts(int opts);

	friend int linebreakc_trampoline(int, char32_t, void *);

	linebreakc_callback_base &operator<<(char32_t uc);

	template<typename iter_type>
	linebreakc_callback_base &operator()(iter_type beg_iter,
					     iter_type end_iter)
	{
		while (beg_iter != end_iter)
			operator<<(*beg_iter++);
		return *this;
	}

	linebreakc_callback_base &operator<<(const
					     std::u32string
					     &vec)
	{
		return operator()(vec.begin(), vec.end());
	}
private:
	virtual int callback(int, char32_t);
};

class linebreakc_callback_save_buf : public linebreakc_callback_base {

public:
	std::list<std::pair<int, char32_t> > lb_buf;

	linebreakc_callback_save_buf();
	~linebreakc_callback_save_buf();

	using linebreakc_callback_base::operator<<;
	using linebreakc_callback_base::operator();
private:
	int callback(int, char32_t);
};


/*
** Convert an input iterator sequence over char32_ts into
** an input iterator sequence over std::pair<int, char32_t>,
** the original unicode character, and the linebreaking value before
** the character.
*/

template<typename input_t> class linebreakc_iter
	: public std::iterator<std::input_iterator_tag,
			       std::pair<int, char32_t>, void>
{
	mutable input_t iter_value, end_iter_value;

	mutable linebreakc_callback_save_buf *buf;

	void fill() const
	{
		if (buf == NULL)
			return;

		while (buf->lb_buf.empty())
		{
			if (iter_value == end_iter_value)
			{
				buf->finish();
				if (buf->lb_buf.empty())
				{
					delete buf;
					buf=NULL;
				}
				break;
			}

			buf->operator<<(*iter_value);
			++iter_value;
		}
	}

	mutable value_type bufvalue;

public:
	linebreakc_iter(const input_t &iter_valueArg,
			const input_t &iter_endvalueArg)
		: iter_value(iter_valueArg),
		  end_iter_value(iter_endvalueArg),
		  buf(new linebreakc_callback_save_buf)
	{
	}

	linebreakc_iter() : buf(NULL)
	{
	}

	~linebreakc_iter()
	{
		if (buf)
			delete buf;
	}

	linebreakc_iter(const linebreakc_iter<input_t> &v)
		: buf(NULL)
	{
		operator=(v);
	}

	linebreakc_iter<input_t> &operator=(const
					    linebreakc_iter<input_t> &v)
	{
		if (buf)
			delete buf;
		buf=v.buf;
		iter_value=v.iter_value;
		end_iter_value=v.end_iter_value;
		v.buf=NULL;
		return *this;
	}

	bool operator==(const linebreakc_iter<input_t> &v) const
	{
		fill();
		v.fill();

		return buf == NULL && v.buf == NULL;
	}

	bool operator!=(const linebreakc_iter<input_t> &v) const
	{
		return !operator==(v);
	}

	value_type operator*() const
	{
		fill();
		return buf == NULL ?
			std::make_pair(UNICODE_LB_MANDATORY,
				       (char32_t)0):
			buf->lb_buf.front();
	}

	linebreakc_iter<input_t> &operator++()
	{
		bufvalue=operator*();

		if (buf)
			buf->lb_buf.pop_front();
		return *this;
	}

	const value_type *operator++(int)
	{
		operator++();
		return &bufvalue;
	}
};


/*
** Subclass wordbreak_callback_base, implement operator()(int).
**
** Use operator<< or operator()(iterator, iterator) to feed
** char32_ts into the wordbreaking algorithm. The subclass receives
** word flags, as they become available.
*/

extern "C" int wordbreak_trampoline(int value, void *ptr);

class wordbreak_callback_base {

	unicode_wb_info_t handle;

#if __cplusplus >= 201103L
public:
	wordbreak_callback_base(const wordbreak_callback_base &)=delete;

	wordbreak_callback_base &operator=(const
					   wordbreak_callback_base &)
	=delete;
private:
#else
	wordbreak_callback_base(const wordbreak_callback_base &);
	/* NOT IMPLEMENTED */

	wordbreak_callback_base &operator=(const
					   wordbreak_callback_base &);
	/* NOT IMPLEMENTED */
#endif
public:
	wordbreak_callback_base();
	virtual ~wordbreak_callback_base();

	void finish();

	friend int wordbreak_trampoline(int, void *);

	wordbreak_callback_base &operator<<(char32_t uc);

	template<typename iter_type>
	wordbreak_callback_base &operator()(iter_type beg_iter,
					    iter_type end_iter)
	{
		while (beg_iter != end_iter)
			operator<<(*beg_iter++);
		return *this;
	}

	wordbreak_callback_base &operator<<(const
					    std::u32string
					    &vec)
	{
		return operator()(vec.begin(), vec.end());
	}
private:
	virtual int callback(bool);
};

/*
** A C++ wrapper for unicode_wbscan.
*/

class wordbreakscan {

	unicode_wbscan_info_t handle;

#if __cplusplus >= 201103L
public:
	wordbreakscan(const wordbreakscan &)=delete;
	wordbreakscan &operator=(const wordbreakscan &)=delete;
private:
#else
	wordbreakscan(const wordbreakscan &);
	/* NOT IMPLEMENTED */

	wordbreakscan &operator=(const wordbreakscan &);
	/* NOT IMPLEMENTED */
#endif
public:

	wordbreakscan();
	~wordbreakscan();

	bool operator<<(char32_t uc);

	size_t finish();
};

//! Convert string in unicode_default_chset() to lowercase

std::string tolower(const std::string &string);

//! Convert string in unicode_default_chset() to uppercase

std::string toupper(const std::string &string);

//! Convert string in the given character set to lowercase

std::string tolower(const std::string &string,
		    const std::string &charset);

//! Convert string in the given character set to uppercase

std::string toupper(const std::string &string,
		    const std::string &charset);

//! Convert unicode to lowercase

std::u32string tolower(const std::u32string &u);

//! Convert unicode to uppercase

std::u32string toupper(const std::u32string &u);

//! Calculate bidirectional character types

//! Passed as a parameter to bidi_calc(), supplying the string and the
//! calculated bidirectional types.

struct bidi_calc_types {
	const std::u32string &s;

	//! Calculated bidirectional types.

	std::vector<enum_bidi_type_t> types;

	//! A reference to an existing std::u32string

	//! bidi_calc_types can be constructed only from a reference to
	//! an existing std::u32string.
	bidi_calc_types(const std::u32string &);

#if __cplusplus >= 201103L
	//! Deleted constructor

	//! bidi_calc_types cannot be constructed from a temporary
	//! std::u32string.
	bidi_calc_types(std::u32string &&)=delete;
#endif
	//! Replace all paragraph breaks by newlines.

	void setbnl(std::u32string &);

	//! Destructor

	~bidi_calc_types();
};

//! Calculate bidirectional embedding levels

//! Returns the bidirectional embedding levels, and the paragraph
//! embedding level.
//!
//! The first parameter can be implicitly converted from an existing
//! std::u32string object. Alternatively a bidi_calc_types helper
//! can be constructed explicitly, and then passed in directly.

std::tuple<std::vector<unicode_bidi_level_t>,
	   struct unicode_bidi_direction> bidi_calc(const bidi_calc_types &s);

//! Calculate bidirectional embedding levels

//! Overload calculates the embedding levels using a predetermined
//! paragraph embedding level.
//!
//! Returns the bidirectional embedding levels, and the same paragraph
//! embedding level.

std::tuple<std::vector<unicode_bidi_level_t>,
	   struct unicode_bidi_direction> bidi_calc(const bidi_calc_types &s,
						    unicode_bidi_level_t level);

//! Reorder bidirectional text

//! Reorders the string and levels in place.
//!
//! Non-0 return value indicates the string and levels' sizes do not match.

int bidi_reorder(std::u32string &string,
		 std::vector<unicode_bidi_level_t> &levels,
		 const std::function<void (size_t, size_t)>
		 &reorder_callback=[](size_t, size_t){},
		 size_t starting_pos=0,
		 size_t n=(size_t)-1);

//! Dry-run reorder bidirectional text
void bidi_reorder(std::vector<unicode_bidi_level_t> &levels,
		  const std::function<void (size_t, size_t)>
		  &reorder_callback=[](size_t, size_t){},
		  size_t starting_pos=0,
		  size_t n=(size_t)-1);

//! Remove directional markers

//! Removes them from the string, in place. Optional lambda gets notified
//! of the index (in the original string, of each removed marker.

void bidi_cleanup(std::u32string &string,
		  const std::function<void (size_t)> &removed_callback=
		  [](size_t) {},
		  int cleanup_options=0);

//! Also remove them from the embedding direction level buffer.

//! Returns non-0 in case of non-matching level buffer size.

int bidi_cleanup(std::u32string &string,
		 std::vector<unicode_bidi_level_t> &levels,
		 const std::function<void (size_t)> &removed_callback=
		 [](size_t) {},
		 int cleanup_options=0);

//! Clean up a substring of the unicode string.

//! The substring gets specified by starting_pos and n.
//!
//! \note
//! The removed position parameter to the removed_callback is based on
//! the given starting_position. Add starting_pos to it to get the
//! actual removed index.
//!
//! Returns non-0 in case of non-matching level buffer size.
//!
//! The final size of the returned string is determined by counting
//! how many calls to removed_callback were made.

int bidi_cleanup(std::u32string &string,
		 std::vector<unicode_bidi_level_t> &levels,
		 const std::function<void (size_t)> &removed_callback,
		 int cleanup_options,
		 size_t starting_pos,
		 size_t n);

//! Convert Unicode string from canonical rendering order to logical order.
int bidi_logical_order(std::u32string &string,
		       std::vector<unicode_bidi_level_t> &levels,
		       unicode_bidi_level_t paragraph_embedding,
		       const std::function<void (size_t, size_t)>
		       &lambda=[](size_t,size_t){},
		       size_t starting_pos=0,
		       size_t n=(size_t)-1);

//! Convert Unicode string from canonical rendering order to logical order.
void bidi_logical_order(std::vector<unicode_bidi_level_t> &levels,
			unicode_bidi_level_t paragraph_embedding,
			const std::function<void (size_t, size_t)> &lambda,
			size_t starting_pos=0,
			size_t n=(size_t)-1);

//! Whether directional and isolation markers are needed.

bool bidi_needs_embed(const std::u32string &string,
		      const std::vector<unicode_bidi_level_t> &levels,
		      const unicode_bidi_level_t *paragraph_embedding=0,
		      size_t starting_pos=0,
		      size_t n=(size_t)-1);

//! Embed directional and isolation markers

//! Non-0 return value indicates the string and levels' sizes do not match.
//!
//! The lambda gets called repeatedly, to specify the contents of the
//! string with embedded direction markers.

int bidi_embed(const std::u32string &string,
	       const std::vector<unicode_bidi_level_t> &levels,
	       unicode_bidi_level_t paragraph_embedding,
	       const std::function<void (const char32_t *string,
					 size_t n,
					 bool is_part_of_string)> &lambda);

//! Embed directional and isolation markers

//! \overload
//!
//! Provides a lambda that collects the new string, and returns it. An
//! empty string gets returned if the string and levels' sizes do not match.

std::u32string bidi_embed(const std::u32string &string,
			  const std::vector<unicode_bidi_level_t> &levels,
			  unicode_bidi_level_t paragraph_embedding);


//! Identify contiguous sequences of combining characters

//! Bounded by each embedding level.

void bidi_combinings(const std::u32string &string,
		     const std::vector<unicode_bidi_level_t> &levels,
		     const std::function<void (unicode_bidi_level_t level,
					       size_t level_start,
					       size_t n_chars,
					       size_t comb_start,
					       size_t n_comb_chars)> &callback);

//! Identify contiguous sequences of composition characters

void bidi_combinings(const std::u32string &string,
		     const std::function<void (unicode_bidi_level_t level,
					       size_t level_start,
					       size_t n_chars,
					       size_t comb_start,
					       size_t n_comb_chars)> &callback);

//! Check if a directional marker needs to be inserted

//! In order for the unicode string to have the specified default
//! paragraph embedding level.

char32_t bidi_embed_paragraph_level(const std::u32string &string,
				    unicode_bidi_level_t level);

//! Compute default direction of text

unicode_bidi_direction bidi_get_direction(const std::u32string &string,
					  size_t starting_pos=0,
					  size_t n=(size_t)-1);

//! Override bidi direction.
std::u32string bidi_override(const std::u32string &s,
			     unicode_bidi_level_t direction,
			     int cleanup_options=0);

constexpr int decompose_flag_qc=UNICODE_DECOMPOSE_FLAG_QC;
constexpr int decompose_flag_compat=UNICODE_DECOMPOSE_FLAG_COMPAT;

void decompose_default_reallocate(std::u32string &,
				  const std::vector<std::tuple<size_t, size_t>>
				  &);

void decompose(std::u32string &,
	       int flags=0,
	       const std::function<void (std::u32string &,
					 const std::vector<std::tuple<size_t,
					 size_t>>)> &
	       =decompose_default_reallocate);

constexpr int compose_flag_removeunused=UNICODE_COMPOSE_FLAG_REMOVEUNUSED;

constexpr int compose_flag_oneshot=UNICODE_COMPOSE_FLAG_ONESHOT;

void compose_default_callback(unicode_composition_t &);

void compose(std::u32string &string,
	     int flags=0,
	     const std::function<void (unicode_composition_t &)> &cb=
	     compose_default_callback);

#if 0
{
#endif
}
#endif

#endif
