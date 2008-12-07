;;
;; menu.scm
;;
;; Copyright (C) 2007, 2008 Francesco Salvestrini
;;                          Alessandro Massignan
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License along
;; with this program; if not, write to the Free Software Foundation, Inc.,
;; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;;

(define node-id
  (lambda (n)
    (car (cadr n))))

(define node-title
  (lambda (n)
   (cadr (cadr n))))

(define node-href
  (lambda (n)
    (car (cddr (cadr n)))))

(define node-children
  (lambda (n)
    (cadr (cddr (cadr n)))))

(define node-is-leaf?
  (lambda (n)
    (equal? (car (cdar n)) 'leaf)))

(define node-is-node?
  (lambda (n)
    (equal? (car (cdar n)) 'node)))

(define leaf-in-here?
  (lambda (l n)
    (call-with-current-continuation
     (lambda (break)
       (do ((n n (cdr n)))
	   ((null? n) #f)
	 (if (pair? n)
	     (if (node-is-leaf? (car n))
		 (if (equal? (node-id (car n)) l)
		     (break #t)))))))))

(define node-in-here?
  (lambda (l n)
    (call-with-current-continuation
     (lambda (break)
       (do ((n n (cdr n)))
	   ((null? n) #f)
	 (if (pair? n)
	     (if (node-is-node? (car n))
		 (if (equal? (node-id (car n)) l)
		     (break #t)))))))))

(define node-in-node?
  (lambda (l n)
    (call-with-current-continuation
     (lambda (break)
       (do ((n n (cdr n)))
	   ((null? n) #f)
	 (if (pair? n)
	     (if (or (leaf-in-here? l n)
		     (node-in-here? l n))
		 (break #t)
		 (if (and (node-is-node? (car n))
			  (node-in-node? l (node-children (car n))))
		     (break #t)))))))))

;; (define tree->sxml
;;   (lambda (l t)
;;     (let ((s '()))
;;       `(ul
;;	,@(call-with-current-continuation
;;	   (lambda (break)
;;	     (do ((t t (cdr t)))
;;		 ((null? t))
;;	       (if (pair? t)
;;		   (if (node-is-node? (car t))
;;		       (if (or (node-in-node? l (node-children (car t)))
;;			       (equal? (node-id (car t)) l))
;;			   (set! s
;;				 (cons
;;				  `(li (a (@ (href ,(node-href (car t))))
;;					  ,(node-title (car t)))
;;				       ,(tree->sxml l (node-children (car t))))
;;				  s))
;;			   (set! s
;;				 (cons
;;				  `(li  (a (@ (href ,(node-href (car t))))
;;					   ,(node-title (car t))))
;;				  s)))
;;		       (set! s
;;			     (cons
;;			      `(li  (a (@ (href ,(node-href (car t))))
;;				       ,(node-title (car t))))
;;			      s)))))
;;	     (reverse s)))))))

(define tree->sxml
  (lambda (l t)
    (let ((s '()))
      `(ul
	,@(call-with-current-continuation
	   (lambda (break)
	     (do ((t t (cdr t)))
		 ((null? t))
	       (if (pair? t)
		   (if (node-is-node? (car t))
		       (set! s
			     (cons
			      `(li (@ (class "father"))
				   (a (@ (href ,(node-href (car t))))
				      ,(node-title (car t)))
				   ,(tree->sxml l (node-children (car t))))
			      s))
		       (set! s
			     (cons
			      `(li (@ (class "child"))
				   (a (@ (href ,(node-href (car t))))
				       ,(node-title (car t))))
			      s)))))
	     (reverse s)))))))

(define pagemap->sxml
  (lambda (p)
    (tree->sxml (car p) (node-children (cadr p)))))

(define map->sxml
  (lambda (in-port out-port)
    (write (pagemap->sxml (read in-port)) out-port)
    ))
