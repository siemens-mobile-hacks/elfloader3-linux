#include <cstring>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <unistd.h>
#include "src/swilib/switab.h"

// #define SWIHOOK_DEBUG

#ifdef SWIHOOK_DEBUG
	#define SWIHOOK_DBG(format, ...) do { \
		int _sz = sprintf(debug_buffer, "[swihook] " format, ##__VA_ARGS__); \
		write(1, debug_buffer, _sz); \
	} while (false)
#else
	#define SWIHOOK_DBG(format, ...)
#endif

typedef void (*jumper_t)();

extern "C" void _jumper_arm_eq();
extern "C" void _jumper_arm_ne();
extern "C" void _jumper_arm_cs();
extern "C" void _jumper_arm_cc();
extern "C" void _jumper_arm_mi();
extern "C" void _jumper_arm_pl();
extern "C" void _jumper_arm_vs();
extern "C" void _jumper_arm_vc();
extern "C" void _jumper_arm_hi();
extern "C" void _jumper_arm_ls();
extern "C" void _jumper_arm_ge();
extern "C" void _jumper_arm_lt();
extern "C" void _jumper_arm_gt();
extern "C" void _jumper_arm_le();
extern "C" void _jumper_arm();
extern "C" void _jumper_thumb();

const jumper_t arm_jumpers[] = {
	_jumper_arm_eq, _jumper_arm_ne, _jumper_arm_cs, _jumper_arm_cc,
	_jumper_arm_mi, _jumper_arm_pl, _jumper_arm_vs, _jumper_arm_vc,
	_jumper_arm_hi, _jumper_arm_ls, _jumper_arm_ge, _jumper_arm_lt,
	_jumper_arm_gt, _jumper_arm_le, _jumper_arm, NULL
};

static void sigill_handler(int signo, siginfo_t *info, void *context) {
	#ifdef SWIHOOK_DEBUG
		static const char *MNEMONICS[] = { "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "", "??" };
		static char debug_buffer[1024] = {};
	#endif

	ucontext_t *uc = (ucontext_t *) context;

	bool is_thumb = (uc->uc_mcontext.arm_cpsr & (1 << 5)) != 0;
	if (is_thumb) {
		uint32_t instr = *((uint32_t *) (uc->uc_mcontext.arm_pc - 2));
		uint32_t imm8 = (instr & 0x00FF);
		uint32_t swi_number = imm8;

		uc->uc_mcontext.arm_pc &= ~1;

		if ((instr & 0xFF00) == 0xDF00) {
			// Extended SWI call
			if (swi_number == 199) {
				swi_number = *((uint32_t *) (uc->uc_mcontext.arm_pc));
				uc->uc_mcontext.arm_pc += 4;
			}

			if ((swi_number & 0xF000) == 0x8000) {
				uc->uc_mcontext.arm_r0 = (uint32_t) SWILIB[swi_number & 0xFFF];
				SWIHOOK_DBG("[T] %08lX: SVC #0x%04X -> PTR  %08X\n", uc->uc_mcontext.arm_pc, swi_number, (uint32_t) uc->uc_mcontext.arm_r0);
			} else {
				uc->uc_mcontext.arm_lr = uc->uc_mcontext.arm_pc | 1;
				uc->uc_mcontext.arm_ip = (uint32_t) SWILIB[swi_number & 0xFFF];
				uc->uc_mcontext.arm_pc = ((uint32_t) _jumper_thumb);
				SWIHOOK_DBG("[T] %08lX: SVC #0x%04X -> CALL %08X\n", uc->uc_mcontext.arm_pc, swi_number, (uint32_t) uc->uc_mcontext.arm_ip);
			}
		} else {
			SWIHOOK_DBG("[T] %08lX: %08X (not SVC)\n", uc->uc_mcontext.arm_pc, instr);
			loader_uninstall_swihook();
			uc->uc_mcontext.arm_pc -= 2;
		}

		// FIXME: Bug in qemu? Need to check on real hardware.
		// thumb_tr_translate_insn: Assertion `(dc->base.pc_next & 1) == 0' failed.
		uc->uc_mcontext.arm_pc &= ~1;
	} else {
		uint32_t instr = *((uint32_t *) (uc->uc_mcontext.arm_pc - 4));
		uint32_t cond = (instr & 0xF0000000) >> 28;
		uint32_t imm24 = (instr & 0x00FFFFFF);

		if ((instr & 0x0F000000) == 0x0F000000 && arm_jumpers[cond]) {
			if ((imm24 & 0xF000) == 0x8000) {
				uc->uc_mcontext.arm_r0 = (uint32_t) SWILIB[imm24 & 0xFFF];
				SWIHOOK_DBG("[A] %08lX: SVC%s #0x%04X -> LOAD 0x%08X\n", uc->uc_mcontext.arm_pc, MNEMONICS[cond], imm24, (uint32_t) uc->uc_mcontext.arm_r0);
			} else {
				uc->uc_mcontext.arm_lr = uc->uc_mcontext.arm_pc;
				uc->uc_mcontext.arm_ip = (uint32_t) SWILIB[imm24 & 0xFFF];
				uc->uc_mcontext.arm_pc = (uint32_t) arm_jumpers[cond];
				SWIHOOK_DBG("[A] %08lX: SVC%s #0x%04X -> CALL 0x%08X\n", uc->uc_mcontext.arm_pc, MNEMONICS[cond], imm24, (uint32_t) uc->uc_mcontext.arm_ip);
			}
		} else {
			SWIHOOK_DBG("[A] %08lX: %08X (not SVC)\n", uc->uc_mcontext.arm_pc, instr);
			loader_uninstall_swihook();
			uc->uc_mcontext.arm_pc -= 4;
		}
	}
}

void loader_install_swihook() {
	stack_t ss = {};
	ss.ss_sp = malloc(SIGSTKSZ);
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;

	if (sigaltstack(&ss, NULL) == -1) {
		spdlog::error("sigaltstack(): {}", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sigaction sa = {};
	sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
	sa.sa_sigaction = sigill_handler;

	if (sigaction(SIGILL, &sa, NULL) == -1) {
		spdlog::error("sigaction(): {}", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void loader_uninstall_swihook() {
	struct sigaction sa = {};
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGILL, &sa, NULL) == -1) {
		spdlog::error("sigaction(): {}", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
