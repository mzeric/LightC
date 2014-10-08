(use-modules (ice-9 format))
(display "begin guile lisp\n")

(define (dis-decl s)
  (begin
    (display "A\n"))
  )

(define (dump-stmts block)
    (format #t "guile basic_block:<~A>>" block)
    (map (lambda(s) 
	   (format #t " (~A:~A)" (node-type s)  s))
	 (block-stmts block))
    (display "\n"))

(define (dump2)
  (display "guile hello\n")
    (display "guile all-blocks\n")
    (for-each dump-stmts (all-blocks entry_block)))

(define (draw-edges block)
  (map (lambda(s)
	 (format #t "\"~A\" -> \"~A\"\n" block s))
       (block-succs block) ))

(define (dump-cfg file)
  (with-output-to-file file
 	(lambda ()
	  (display "digraph CFG {\n")
	  (map (lambda (s) (draw-edges s ))
	       (all-blocks entry_block))
	  (display "}\n"))))
;; pretty the output of AST-Node
;; walk-list , output a new list with (func element) each of lists
(define (node-info s)
  (format #f "~A:~A" (node-type s) s))
(define (walk-list func lists)
  (cond ((not (list? lists)) 
	 (node-info lists))
      	((null? lists)
	 '())
      	(else
	  (cons (walk-list func (car lists)) (walk-list func (cdr lists))))))
(define (pretty-tree lists)
  (walk-list node-info lists))

;; convert lisp-format to dot
(define (draw-dot from to)
  (format #t "\"~A\" -> \"~A\"\n" from to))
(define (_lisp->dot lists)
  (for-each (lambda (s)
  	      (if (list? s)
  	      	(begin
		  (draw-dot (car lists) (car s))
		  (_lisp->dot s))
		(draw-dot (car lists) s))) (cdr lists)))
;; wrap to write dot-file
(define (lisp->dot filename lists)
  (with-output-to-file filename
 	(lambda ()
	  (display "digraph AST {\n")
	  (_lisp->dot lists)
	  (display "}\n"))))
(define (block->dot filename lists)
  (with-output-to-file filename
 	(lambda ()
	  (display "digraph BLOCK {\n")
	  (for-each _lisp->dot lists)
	  (display "}\n"))))

(define (tp )
  (map (lambda(e)
  	 (begin
	 (format #t "guile-expand ~A = ~A\n" e (expand-expr  e))
	 (format #t "guile-pretty ~A\n" (pretty-tree (expand-expr e)))
	 (lisp->dot "ast.dot" (pretty-tree (expand-expr e)))))
       (block-stmts (caddr (all-blocks entry_block)))))
(define (print-basic-block bb)
  (map (lambda (e)
	 (pretty-tree (expand-expr e)))
	     (block-stmts bb)))

(define (tp2)
	(print-basic-block (caddr (cdddr (all-blocks entry_block)))))

(define (block_func)
    (dump2)
    (dump-cfg "cfg.dot")
    (tp)
    (block->dot "all.dot" (tp2))
    (format #t "guile-end: ~A\n" (tp2)))
