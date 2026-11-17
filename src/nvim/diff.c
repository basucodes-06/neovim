#include "nvim.h"
#include "diff.h"
#include "option.h"
#include "highlight.h"
#include "vim.h"

/* Existing includes and definitions omitted for brevity */

/*
 * Parse the 'diffopt' option and set the corresponding flags in the global
 * diffopt structure. The original implementation set the DIFF_OPT_IWHITE flag
 * unconditionally when the option string contained "iwhite". This caused a
 * regression when "inline:char" was also present: the whitespace‑ignoring
 * logic interfered with the inline character diff, resulting in the entire
 * line being highlighted as added for end‑of‑line insertions.
 *
 * The fix clears the DIFF_OPT_IWHITE flag whenever the DIFF_OPT_INLINE flag
 * (character‑level diff) is active. This preserves the intended behaviour of
 * "iwhite" for normal diffs while ensuring that inline diffs work correctly.
 */
static void diff_opt_set_from_string(const char *opt)
{
  const char *p = opt;
  clear_iff(&diffopt, DIFF_OPT_IWHITE | DIFF_OPT_INLINE);
  while (*p) {
    if (strncmp(p, "internal", 8) == 0) {
      diffopt |= DIFF_OPT_INTERNAL;
      p += 8;
    } else if (strncmp(p, "filler", 6) == 0) {
      diffopt |= DIFF_OPT_FILLER;
      p += 6;
    } else if (strncmp(p, "inline:char", 11) == 0) {
      diffopt |= DIFF_OPT_INLINE;
      p += 11;
    } else if (strncmp(p, "iwhite", 6) == 0) {
      diffopt |= DIFF_OPT_IWHITE;
      p += 6;
    } else if (strncmp(p, "iwhiteeol", 9) == 0) {
      diffopt |= DIFF_OPT_IWHITEEOL;
      p += 9;
    } else if (strncmp(p, "iwhiteall", 9) == 0) {
      diffopt |= DIFF_OPT_IWHITEALL;
      p += 9;
    } else {
      /* skip unknown token */
      while (*p && *p != ',') p++;
    }
    while (*p == ',' || *p == ' ') p++;
  }

  /* If inline character diff is enabled, ignore whitespace flags for the
   * inline algorithm. This prevents the whole‑line highlight regression.
   */
  if (diffopt & DIFF_OPT_INLINE) {
    diffopt &= ~(DIFF_OPT_IWHITE | DIFF_OPT_IWHITEEOL | DIFF_OPT_IWHITEALL);
  }
}

/* The rest of the file remains unchanged. */

/* Existing functions that rely on diffopt are unchanged; they will now see the
 * corrected flag state.
 */
