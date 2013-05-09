/* vim:set et sts=4: */

#include "engine.h"
#include "string.h"
#include "callbacks.h"
#include "debug.h"

typedef struct _IBusMyansanEngine IBusMyansanEngine;
typedef struct _IBusMyansanEngineClass IBusMyansanEngineClass;

struct _IBusMyansanEngine {
	IBusEngine parent;

	guint		cur_pos;
	gboolean	third_level_on;
	gunichar	buffer[8];
};

struct _IBusMyansanEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void ibus_myansan_engine_class_init	(IBusMyansanEngineClass *klass);
static void ibus_myansan_engine_init	(IBusMyansanEngine	*engine);
static void ibus_myansan_engine_destroy (IBusMyansanEngine	*engine);
static void ibus_myansan_engine_enable	(IBusEngine		*engine);

static gboolean ibus_myansan_engine_process_key_event (IBusEngine	*engine,
							   guint		keyval,
							   guint		keycode,
							   guint		modifiers);

G_DEFINE_TYPE (IBusMyansanEngine, ibus_myansan_engine, IBUS_TYPE_ENGINE)

static void
ibus_myansan_engine_class_init (IBusMyansanEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_myansan_engine_destroy;
	engine_class->process_key_event = ibus_myansan_engine_process_key_event;
}

static void
ibus_myansan_engine_init (IBusMyansanEngine *myansan)
{
	rookie_debug_init ();
	rookie_debug ("Starting IBus Burmese ...");

	ibus_engine_get_surrounding_text (IBUS_ENGINE(myansan), NULL, NULL, NULL);
	g_signal_connect (myansan, "enable", G_CALLBACK(ibus_myansan_engine_enable), NULL);
}

static void
ibus_myansan_engine_enable (IBusEngine *engine)
{
	IBusText *text = NULL;
	guint cur_pos;

	ibus_engine_get_surrounding_text (engine, NULL, NULL, NULL);

	if ((engine->client_capabilities & IBUS_CAP_SURROUNDING_TEXT) == 0 ) {
		rookie_debug ("IBus Surrounding Text API is not supported.");
	} else {
		rookie_debug ("IBus Surrounding Text API is supported.");
	}
}

static void
ibus_myansan_engine_destroy (IBusMyansanEngine *myansan)
{
	rookie_debug_exit ();
	((IBusObjectClass *) ibus_myansan_engine_parent_class)->destroy ((IBusObject *)myansan);
}

static gboolean g_unichar_is_burmese_consonant (gunichar chr)
{
	return (chr >= MYANMAR_LETTER_KA && chr <= MYANMAR_LETTER_A);
}

static gboolean g_unichar_is_burmese_medial (gunichar chr)
{
	return (chr >= MYANMAR_CONSONANT_SIGN_MEDIAL_YA && chr <= MYANMAR_CONSONANT_SIGN_MEDIAL_HA);
}

static gboolean
ibus_myansan_engine_process_key_event (IBusEngine *engine,
									   guint	   keyval,
									   guint	   keycode,
									   guint	   modifiers)
{
	if (modifiers & IBUS_RELEASE_MASK)
		return FALSE;

	modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

	if (modifiers != 0)
		return FALSE;

	IBusText *text;
	IBusMyansanEngine *myansan = (IBusMyansanEngine *)engine;

	gint anchor_pos = 0;
	ibus_engine_get_surrounding_text (engine, &text, &myansan->cur_pos, &anchor_pos);
	//	g_object_unref (text);

	const gchar *surrounding_text = ibus_text_get_text (text);
	rookie_debug ( "surrounding_text = %s\n", surrounding_text);

	gunichar prev_char;
	gchar *tmp;
	gint size = 1;
	memset (myansan->buffer, '\0', 8 * sizeof (gunichar));

	switch (keyval) {
	case IBUS_grave:
		myansan->third_level_on = TRUE;
		return FALSE;

	case IBUS_asciitilde:
		myansan->buffer[0] = MYANMAR_LETTER_DDHA;
		break;

	case IBUS_1:
		myansan->buffer[0] = MYANMAR_DIGIT_ONE;
		break;

	case IBUS_2:
		myansan->buffer[0] = MYANMAR_DIGIT_TWO;
		break;

	case IBUS_3:
		myansan->buffer[0] = MYANMAR_DIGIT_THREE;
		break;

	case IBUS_4:
		myansan->buffer[0] = MYANMAR_DIGIT_FOUR;
		break;

	case IBUS_5:
		myansan->buffer[0] = MYANMAR_DIGIT_FIVE;
		break;

	case IBUS_6:
		myansan->buffer[0] = MYANMAR_DIGIT_SIX;
		break;

	case IBUS_7:
		myansan->buffer[0] = MYANMAR_DIGIT_SEVEN;
		break;

	case IBUS_8:
		myansan->buffer[0] = MYANMAR_DIGIT_EIGHT;
		break;

	case IBUS_9:
		myansan->buffer[0] = MYANMAR_DIGIT_NINE;
		break;

	case IBUS_0:
		myansan->buffer[0] = MYANMAR_DIGIT_ZERO;
		break;

	case IBUS_minus:
		myansan->buffer[0] = MINUS_SIGN;
		break;

	case IBUS_equal:
		myansan->buffer[0] = EQUALS_SIGN;
		break;

	case IBUS_exclam:
		myansan->buffer[0] = MYANMAR_LETTER_DDA;
		break;

	case IBUS_at:
		myansan->buffer[0] = MYANMAR_LETTER_NNA;
		myansan->buffer[1] = MYANMAR_SIGN_VIRAMA;
		myansan->buffer[2] = myansan->third_level_on ?
			MYANMAR_LETTER_TTHA :
			MYANMAR_LETTER_DDA;
		size = 3;
		break;

	case IBUS_numbersign:
		myansan->buffer[0] = MYANMAR_LETTER_TTA;
		break;

	case IBUS_dollar:
		myansan->buffer[0] = MYANMAR_LETTER_KA;
		myansan->buffer[1] = MYANMAR_CONSONANT_SIGN_MEDIAL_YA;
		myansan->buffer[2] = MYANMAR_LETTER_PA;
		myansan->buffer[3] = MYANMAR_SIGN_ASAT;
		size = 4;
		break;

	case IBUS_percent:
		myansan->buffer[0] = PERCENT_SIGN;
		break;

	case IBUS_asciicircum:
		myansan->buffer[0] = SOLIDUS;
		break;

	case IBUS_ampersand:
		myansan->buffer[0] = MYANMAR_LETTER_RA;
		break;

	case IBUS_asterisk:
		myansan->buffer[0] = MYANMAR_LETTER_GA;
		break;

	case IBUS_parenleft:
		myansan->buffer[0] = LEFT_PARENTHESIS;
		break;

	case IBUS_parenright:
		myansan->buffer[0] = RIGHT_PARENTHESIS;
		break;

	case IBUS_underscore:
		myansan->buffer[0] = SUMMATION;
		break;

	case IBUS_plus:
		myansan->buffer[0] = PLUS_SIGN;
		break;

	case IBUS_q:
		myansan->buffer[0] = MYANMAR_LETTER_CHA;
		break;

	case IBUS_Q:
		myansan->buffer[0] = MYANMAR_LETTER_JHA;
		break;

	case IBUS_w:
		myansan->buffer[0] = MYANMAR_LETTER_TA;
		break;

	case IBUS_W:
		myansan->buffer[0] = MYANMAR_LETTER_WA;
		break;

	case IBUS_e:
		myansan->buffer[0] = MYANMAR_LETTER_NA;
		break;

	case IBUS_E:
		myansan->buffer[0] = MYANMAR_LETTER_I;
		break;

	case IBUS_r:
		myansan->buffer[0] = MYANMAR_LETTER_MA;
		break;

	case IBUS_R:
		myansan->buffer[0] = MYANMAR_SYMBOL_AFOREMENTIONED;
		myansan->buffer[1] = MYANMAR_LETTER_NGA;
		myansan->buffer[2] = MYANMAR_SIGN_ASAT;
		myansan->buffer[3] = MYANMAR_SIGN_VISARGA;
		size = 4;
		break;

	case IBUS_t:
		myansan->buffer[0] = MYANMAR_LETTER_A;
		break;

	case IBUS_T:
		myansan->buffer[0] = MYANMAR_LETTER_II;
		break;

	case IBUS_y:
		myansan->buffer[0] = MYANMAR_LETTER_PA;
		break;

	case IBUS_Y:
		myansan->buffer[0] = MYANMAR_SYMBOL_LOCATIVE;
		break;

	case IBUS_u:
		myansan->buffer[0] = MYANMAR_LETTER_KA;
		break;

	case IBUS_U:
		myansan->buffer[0] = MYANMAR_LETTER_U;
		break;

	case IBUS_i:
		myansan->buffer[0] = MYANMAR_LETTER_NGA;
		break;

	case IBUS_I:
		myansan->buffer[0] = MYANMAR_SYMBOL_COMPLETED;
		break;

	case IBUS_o:
		myansan->buffer[0] = MYANMAR_LETTER_SA;
		break;

	case IBUS_O:
		myansan->buffer[0] = MYANMAR_LETTER_GREAT_SA;
		break;

	case IBUS_p:
		myansan->buffer[0] = MYANMAR_LETTER_CA;
		break;

	case IBUS_P:
		myansan->buffer[0] = MYANMAR_LETTER_NNA;
		break;

	case IBUS_bracketleft:
		myansan->buffer[0] = MYANMAR_LETTER_HA;
		break;

	case IBUS_bracketright:
		myansan->buffer[0] = MYANMAR_LETTER_O;
		break;

	case IBUS_braceleft:
		myansan->buffer[0] = MYANMAR_LETTER_E;
		break;

	case IBUS_braceright:
		myansan->buffer[0] = MYANMAR_LETTER_AU;
		break;

	case IBUS_a:
		myansan->buffer[0] = ZERO_WIDTH_NON_JOINER;
		myansan->buffer[1] = MYANMAR_VOWEL_SIGN_E;
		size = 2;
		break;

	case IBUS_A:
		myansan->buffer[0] = MYANMAR_LETTER_BA;
		break;

	case IBUS_s:
		myansan->buffer[0] = MYANMAR_CONSONANT_SIGN_MEDIAL_YA;
		break;

	case IBUS_S:
		myansan->buffer[0] = MYANMAR_CONSONANT_SIGN_MEDIAL_HA;
		break;

	case IBUS_d:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_I;
		break;

	case IBUS_D:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_II;
		break;

	case IBUS_f:
		myansan->buffer[0] = MYANMAR_SIGN_ASAT;
		break;

	case IBUS_F:
		myansan->buffer[0] = MYANMAR_SIGN_VIRAMA;
		break;

	case IBUS_g:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_TALL_AA;
		break;

	case IBUS_G:
		myansan->buffer[0] = MYANMAR_CONSONANT_SIGN_MEDIAL_WA;
		break;

	case IBUS_h:
		myansan->buffer[0] = MYANMAR_SIGN_DOT_BELOW;
		break;

	case IBUS_H:
		myansan->buffer[0] = MYANMAR_SIGN_ANUSVARA;
		break;

	case IBUS_j:
		myansan->buffer[0] = MYANMAR_CONSONANT_SIGN_MEDIAL_RA;
		break;

	case IBUS_J:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_AI;
		break;

	case IBUS_k:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_U;
		break;

	case IBUS_K:
		myansan->buffer[0] = MYANMAR_LETTER_DA;
		break;

	case IBUS_l:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_UU;
		break;

	case IBUS_L:
		myansan->buffer[0] = MYANMAR_LETTER_DHA;
		break;

	case IBUS_semicolon:
		myansan->buffer[0] = MYANMAR_SIGN_VISARGA;
		break;

	case IBUS_colon:
		myansan->buffer[0] = COLON;
		break;

	case IBUS_apostrophe:
		/* Check which one to put, opening or close single quote */
		tmp = g_utf8_find_prev_char (surrounding_text, surrounding_text+myansan->cur_pos);
		while (TRUE)
		{
			if (tmp == NULL) {
				myansan->buffer[0] = LEFT_SINGLE_QUOTE;
				break;
			}

			prev_char = g_utf8_get_char (tmp);
			if (prev_char == RIGHT_SINGLE_QUOTE) {
				myansan->buffer[0] = LEFT_SINGLE_QUOTE;
				break;
			}
			if (prev_char == LEFT_SINGLE_QUOTE) {
				myansan->buffer[0] = RIGHT_SINGLE_QUOTE;
				break;
			}
			tmp = g_utf8_find_prev_char (surrounding_text, tmp);
		}
		break;

	case IBUS_quotedbl:
		/* Check which one to put, opening or close double quote */
		tmp = g_utf8_find_prev_char (surrounding_text,
									 surrounding_text+myansan->cur_pos);
		while (TRUE)
		{
			if (tmp == NULL) {
				myansan->buffer[0] = LEFT_DOUBLE_QUOTE;
				break;
			}

			prev_char = g_utf8_get_char (tmp);
			if (prev_char == RIGHT_DOUBLE_QUOTE) {
				myansan->buffer[0] = LEFT_DOUBLE_QUOTE;
				break;
			}
			if (prev_char == LEFT_DOUBLE_QUOTE) {
				myansan->buffer[0] = RIGHT_DOUBLE_QUOTE;
				break;
			}
			tmp = g_utf8_find_prev_char (surrounding_text, tmp);
		}
		break;

	case IBUS_backslash:
		myansan->buffer[0] = MYANMAR_SYMBOL_GENITIVE;
		break;

	case IBUS_bar:
		myansan->buffer[0] = MYANMAR_LETTER_NYA;
		break;

	case IBUS_z:
		myansan->buffer[0] = MYANMAR_LETTER_PHA;
		break;

	case IBUS_Z:
		myansan->buffer[0] = MYANMAR_LETTER_JA;
		break;

	case IBUS_x:
		myansan->buffer[0] = MYANMAR_LETTER_THA;
		break;

	case IBUS_X:
		myansan->buffer[0] = MYANMAR_LETTER_TTHA;
		break;

	case IBUS_c:
		myansan->buffer[0] = MYANMAR_LETTER_KHA;
		break;

	case IBUS_C:
		myansan->buffer[0] = MYANMAR_LETTER_GHA;
		break;

	case IBUS_v:
		myansan->buffer[0] = MYANMAR_LETTER_LA;
		break;

	case IBUS_V:
		myansan->buffer[0] = MYANMAR_LETTER_LLA;
		break;

	case IBUS_b:
		myansan->buffer[0] = MYANMAR_LETTER_BHA;
		break;

	case IBUS_B:
		return TRUE;

	case IBUS_n:
		myansan->buffer[0] = MYANMAR_LETTER_NYA;
		break;

	case IBUS_N:
		myansan->buffer[0] = MYANMAR_LETTER_NNYA;
		break;

	case IBUS_m:
		myansan->buffer[0] = MYANMAR_VOWEL_SIGN_AA;
		break;

	case IBUS_M:
		myansan->buffer[0] = MYANMAR_LETTER_UU;
		break;

	case IBUS_comma:
		myansan->buffer[0] = MYANMAR_LETTER_YA;
		break;

	case IBUS_period:
		myansan->buffer[0] = FULL_STOP;
		break;

	case IBUS_slash:
		myansan->buffer[0] = MYANMAR_SIGN_SECTION;
		break;

	case IBUS_question:
		myansan->buffer[0] = MYANMAR_SIGN_LITTLE_SECTION;
		break;

	default:
		return FALSE;
	}

//	gunichar *tmp32 = g_ut8_to_ucs4 (sur
	tmp = g_utf8_find_prev_char (surrounding_text,
								 surrounding_text+myansan->cur_pos);

	if (tmp != NULL) {
		prev_char = g_utf8_get_char (tmp);
		rookie_debug ("Previous char: %lc\n", prev_char);

		if (prev_char == MYANMAR_VOWEL_SIGN_E) {
			/* if previous char is VOWLE E, switch place with current consonant or medial */
			tmp = g_utf8_find_prev_char (surrounding_text, tmp);

			if (tmp != NULL && g_utf8_get_char (tmp) == ZERO_WIDTH_NON_JOINER &&
				 size == 1 && g_unichar_is_burmese_consonant(myansan->buffer[0])) {
				/* If there is ZWNJ infront of vowel e, it is unattached, so
				 * delete ZWNJ, put vowel e behind consonant */
				ibus_engine_delete_surrounding_text (IBUS_ENGINE (myansan), -2, 2);
				myansan->buffer[1] = MYANMAR_VOWEL_SIGN_E;
				size = 2;
			} else if (g_unichar_is_burmese_medial(myansan->buffer[0]) && size == 1) {
				/* For medials, if there is vowel e infront, swap place */
				ibus_engine_delete_surrounding_text	 (IBUS_ENGINE (myansan), -1, 1);
				myansan->buffer[1] = MYANMAR_VOWEL_SIGN_E;
				size = 2;
			}
		}

		if (prev_char == MYANMAR_SIGN_ASAT && size == 1 && myansan->buffer[0] == MYANMAR_SIGN_DOT_BELOW) {
			/* Swap if ASAT is infront of DOT BELOW */
			ibus_engine_delete_surrounding_text	 (IBUS_ENGINE (myansan), -1, 1);
			myansan->buffer[1] = MYANMAR_SIGN_ASAT;
			size = 2;
		}
	}
	else
		rookie_debug ("tmp is NULL.\n");

	myansan->third_level_on = FALSE;
	myansan->buffer[size] = L'\0';

	IBusText *commit_text;
	commit_text = ibus_text_new_from_ucs4 (myansan->buffer);
	ibus_engine_commit_text ((IBusEngine *)myansan, commit_text);

	return TRUE;
}

