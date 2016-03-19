lui $s0, 65535      		# trying to write a signed overflow test but not working
ori $s0, $s0, 65535          # s0 = 4294967295
lui $s1, 65535      		
ori $s1, $s0, 65535          # s1 = 4294967295
add $s0 $s0 $s1				#overflow?
