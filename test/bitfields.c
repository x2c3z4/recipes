#include <stdio.h>
#include <stdint.h>
/*
 * SPC-3 7.4.6 Table 245
 */
#define  DECL_BITFIELD2(_a, _b)        \
 uint8_t _a, _b
#define  DECL_BITFIELD3(_a, _b, _c)      \
 uint8_t _a, _b, _c
#define  DECL_BITFIELD4(_a, _b, _c, _d)      \
 uint8_t _a, _b, _c, _d
#define  DECL_BITFIELD5(_a, _b, _c, _d, _e)    \
 uint8_t _a, _b, _c, _d, _e
#define  DECL_BITFIELD6(_a, _b, _c, _d, _e, _f)    \
 uint8_t _a, _b, _c, _d, _e, _f
#define  DECL_BITFIELD7(_a, _b, _c, _d, _e, _f, _g)  \
 uint8_t _a, _b, _c, _d, _e, _f, _g
#define  DECL_BITFIELD8(_a, _b, _c, _d, _e, _f, _g, _h)  \
 uint8_t _a, _b, _c, _d, _e, _f, _g, _h

typedef struct spc3_mode_params_control {
 DECL_BITFIELD6(
     mpc_rlec    :1,
     mpc_gltsd   :1,
     mpc_d_sense   :1,
     _reserved1    :1,
     mpc_tmf_only  :1,
     mpc_tst   :3);
 DECL_BITFIELD4(
     _reserved2    :1,
     mpc_q_err   :2,
     _reserved3    :1,
     mpc_queue_alg_mod :4);
 DECL_BITFIELD5(
     _reserved4    :3,
     mpc_swp   :1,
     mpc_ua_intlck_ctrl  :2,
     mpc_rac   :1,
     mpc_vs_4_7    :1);
 DECL_BITFIELD4(
     mpc_autoload_mode :3,
     _reserved5    :3,
     mpc_tas   :1,
     mpc_ato   :1);
 uint8_t _reserved6[2];
 uint16_t mpc_busy_timeout_period;
 uint16_t mpc_ext_selftest_completion_time;
} spc3_mode_page_params_control_t;


#define Res 0
int main()
{
    spc3_mode_page_params_control_t d, *dp;
    d=(spc3_mode_page_params_control_t){
      0,1,0,Res,0,0x5,
      Res,-2,0,0x7,
      Res,1,3,1,1,
      0x7,Res,1,1,
      Res,Res, 0x7fff,0x7fff
    };

    dp = &d;
    printf("RLEC: %d  GLTSD: %d        D_SENSE : %d   TMF_ONLY: %d   TST: %d\n"
    "QERR: %d  QUEUE ALGORITHM MODIFIER: %d   SWP     : %d\n"
    "UA_INTLCK_CTRL: %d   RAC: %d     VS: %d   AUTOLOAD MODE    : %d\n"
    "TAS : %d             ATO: %d           BUSY TIMEOUT PERIOD: %d\n"
    "EXTENDED SELF-TEST COMPLETION TIME             : %d\n\n",
    dp->mpc_rlec, dp->mpc_gltsd, dp->mpc_d_sense, dp->mpc_tmf_only,
    dp->mpc_tst, dp->mpc_q_err, dp->mpc_queue_alg_mod, dp->mpc_swp,
    dp->mpc_ua_intlck_ctrl, dp->mpc_rac, dp->mpc_vs_4_7,
    dp->mpc_autoload_mode, dp->mpc_tas, dp->mpc_ato,
    dp->mpc_busy_timeout_period,
    dp->mpc_ext_selftest_completion_time);
    return 0;
}
