@ arm jumpers

.arm
.global _jumper_arm_eq
.type _jumper_arm_eq, function
_jumper_arm_eq:
	BXEQ IP

.arm
.global _jumper_arm_ne
.type _jumper_arm_ne, function
_jumper_arm_ne:
	BXNE IP

.arm
.global _jumper_arm_cs
.type _jumper_arm_cs, function
_jumper_arm_cs:
	BXCS IP

.arm
.global _jumper_arm_cc
.type _jumper_arm_cc, function
_jumper_arm_cc:
	BXCC IP

.arm
.global _jumper_arm_mi
.type _jumper_arm_mi, function
_jumper_arm_mi:
	BXMI IP

.arm
.global _jumper_arm_pl
.type _jumper_arm_pl, function
_jumper_arm_pl:
	BXPL IP

.arm
.global _jumper_arm_vs
.type _jumper_arm_vs, function
_jumper_arm_vs:
	BXVS IP

.arm
.global _jumper_arm_vc
.type _jumper_arm_vc, function
_jumper_arm_vc:
	BXVC IP

.arm
.global _jumper_arm_hi
.type _jumper_arm_hi, function
_jumper_arm_hi:
	BXHI IP

.arm
.global _jumper_arm_ls
.type _jumper_arm_ls, function
_jumper_arm_ls:
	BXLS IP

.arm
.global _jumper_arm_ge
.type _jumper_arm_ge, function
_jumper_arm_ge:
	BXGE IP

.arm
.global _jumper_arm_lt
.type _jumper_arm_lt, function
_jumper_arm_lt:
	BXLT IP

.arm
.global _jumper_arm_gt
.type _jumper_arm_gt, function
_jumper_arm_gt:
	BXGT IP

.arm
.global _jumper_arm_le
.type _jumper_arm_le, function
_jumper_arm_le:
	BXLE IP

.arm
.global _jumper_arm
.type _jumper_arm, function
_jumper_arm:
	BX IP

@ thumb jumpers

.thumb
.global _jumper_thumb
.type _jumper_thumb, function
_jumper_thumb:
	BX IP
