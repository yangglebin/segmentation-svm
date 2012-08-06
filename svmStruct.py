 

class StructSVM(object):
    def __init__(
            self, 
            training_set, 
            loss_function, 
            psi, 
            most_violated_constraint,
            **kwargs):
        ''' 
            S = [(x1,y1),..., (xn,yn)]
            
            args:
                loss    : (scalar) = loss(y,y_)
                psi     : (vector) = psi(x,y)
                most_violated_constraint : y_ = most_violated_constraint(w,x,y)
        '''
        self.S = training_set
        self.C = kwargs.pop('C',1.)
        self.w_loss  = kwargs.pop('w_loss',1.)
        self.epsilon = kwargs.pop('epsilon',1e-5)
        
        self.loss = loss_function
        self.psi  = psi
        self.mvc  = most_violated_constraint
        
        
    
    def _current_solution(self, W):
        ''' quadratic programming 
        min ztPz + 2*ztq 
            st. Az <= b
        
        Here:
        min 1/2wtw + Cxi
            st. for all (y1_,...yn_) in W
                1/n wt sum(k){psi(xk,yk) - psi(xk,yk_)} 
                    >= 1/n sum(k) {loss(yk,yk_) - xi
        '''
        
        ...
        return w,xi
        
    def _stop_condition(self,w,xi):
        pass
        
    def train(self):
        ''' optimize with algorithm:
        "Cutting-plane training of structural SVMs"
        Machine Learning 2009
        Joachims, Thorsten
        Finley, Thomas 
        Yu, Chun-Nam John
        '''
        
        ## test set for qp
        W = [] 
        
        niter = 0
        while 1:
            ## compute current solution (qp + constraints)
            w,xi = self._current_solution(W)
        
            ## find most violated constraint
            ys = []
            for s in self.S:
                y_ = self.mvc(w, *s)
                ys.append(y_)
                
            ## add to test set
            W.append(ys)
            
            ## stop condition
            if self._stop_condition(w,xi): break
            else: niter+= 1
        
        ## return values
        info = {
            'number of iterations': niter, 
            'number of contraints': len(W),
            }
        return w, xi, info
        
        
if __name__=='__main__':
    
    ## test svm struct
    
    pass
    