(display "begin guile lisp\n")

(define (dump)
  (lambda()
    (display "guile basic_block:")
    (map (lambda(s) 
	   (format #t " (~A)" (node-type s)))
	 (block-stmts entry_block))
    (display "\n")
    ))

(define block_func
  (dump))
