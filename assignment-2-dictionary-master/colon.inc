%define NEXT 0

%macro colon 2
	%2: 
		dq	NEXT
		db	%1, 0
	%define NEXT %2
%endmacro
