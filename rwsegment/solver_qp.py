import numpy as np
from scipy import sparse
from scipy.sparse import linalg as splinalg

def solve_qp(P,q,**kwargs):
    '''
        solve: min(X): Xt*P*X + 2Xt*q + cst
    '''
    
    if P.nnz==0:
        logger.error('P has no non-zeros entries')
        return np.zeros(q.size)
    
    maxiter = int(kwargs.pop('maxiter',1000))
    tol = kwargs.pop('tol',1e-3)
    
    ## use standard conjugate gradient
    x,info = splinalg.cg(
        P,-q,
        maxiter=maxiter,
        tol=tol,
        )
        
    if info!=0:
        logger.error('QP did not converge. info={}'.format(info))
        
    return x

    
import utils_logging
logger = utils_logging.get_logger('solver_qp',utils_logging.DEBUG)