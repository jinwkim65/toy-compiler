
.globl foo
foo:
pushq %rbp
movq %rsp, %rbp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
subq $96, %rsp
movq -16(%rbp), %rax
movq $7, %rcx
cqto
idivq %rcx
movq %rax, -24(%rbp)
movq $7, %rax
movq -24(%rbp), %rcx
imulq %rcx, %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
movq -8(%rbp), %rcx
addq %rcx, %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rax
movq -16(%rbp), %rcx
subq %rcx, %rax
movq %rax, -48(%rbp)
movq -48(%rbp), %rax
movq -40(%rbp), %rcx
andq %rcx, %rax
movq %rax, -56(%rbp)
movq -48(%rbp), %rax
movq -56(%rbp), %rcx
orq %rcx, %rax
movq %rax, -64(%rbp)
movq -64(%rbp), %rax
movq -56(%rbp), %rcx
xorq %rcx, %rax
movq %rax, -72(%rbp)
movq -72(%rbp), %rax
movq $2, %rcx
sarq %rcx, %rax
movq %rax, -80(%rbp)
movq -80(%rbp), %rax
movq $2, %rcx
salq %rcx, %rax
movq %rax, -88(%rbp)
movq -88(%rbp), %rax
negq %rax
movq %rax, -96(%rbp)
movq -96(%rbp), %rax
addq $96, %rsp
popq  %rbp
retq
