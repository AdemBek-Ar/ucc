OPC(PLUS_EQ) OPC(PLUS) OPC(ADD_EQ) OPC(ADD)
    result= *LD(f) + *RD(f);
    break;
OPC(MINUS_EQ) OPC(MINUS) OPC(SUB_EQ) OPC(SUB)
    result= *LD(f) - *RD(f);
    break;

OPC(MUL_EQ) OPC(MUL)
    result= *LD(f) * *RD(f);
    break;
OPC(DIV_EQ) OPC(DIV)
    result= *LD(f) / *RD(f);
    break;
OPC(POW_EQ) OPC(POW) {
    result.set_prec(LD(f)->get_prec());
    mpfr_t m_base, m_exp, m_res;
    mpfr_init2(m_base, LD(f)->get_prec());
    mpfr_init2(m_exp, RD(f)->get_prec());
    mpfr_init2(m_res, result.get_prec());
    mpfr_set_f(m_base, LD(f)->get_mpf_t(), MPFR_RNDN);
    mpfr_set_f(m_exp, RD(f)->get_mpf_t(), MPFR_RNDN);
    mpfr_pow(m_res, m_base, m_exp, MPFR_RNDN);
    mpfr_get_f(result.get_mpf_t(), m_res, MPFR_RNDN);
    mpfr_clears(m_base, m_exp, m_res, NULL);
    break;
}
OPC(ASSEGN)
    LEX.pwrg("assign an lvalue", exp->opl->loc, exp->opl->len);
    result= *RD(f);
    break;

OPC(AND_EQ) OPC(AND)
    result= int(*LD(f) && *RD(f));
    break;
OPC(OR_EQ) OPC(OR)
    result= int(*LD(f) || *RD(f));
    break;
OPC(EQ)
    result= int(*LD(f) == *RD(f));
    break;
OPC(NOT_EQ)
    result= int(*LD(f) != *RD(f));
    break;
OPC(LESS)
    result= int(*LD(f) < *RD(f));
    break;
OPC(GREADER)
    result= int(*LD(f) > *RD(f));
    break;
OPC(LEQ)
    result= int(*LD(f) <= *RD(f));
    break;
OPC(GEQ)
    result= int(*LD(f) >= *RD(f));
    break;
default:
    LEX.perr("unsupported operation", exp->loc, exp->len);
    return false;
