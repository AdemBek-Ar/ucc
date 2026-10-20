OPC(PLUS_EQ) OPC(PLUS) OPC(ADD_EQ) OPC(ADD)
    iv= *LD(i) + *RD(i);
    break;
OPC(MINUS_EQ) OPC(MINUS) OPC(SUB_EQ) OPC(SUB)
    iv= *LD(i) - *RD(i);
    break;

OPC(MUL_EQ) OPC(MUL)
    iv= *LD(i) * *RD(i);
    break;
OPC(DIV_EQ) OPC(DIV) {
    fv= gmp::fbig(*LD(i))/gmp::fbig(*RD(i));
    gmp::fbig_to_ibig(iv.get_mpz_t(), fv.get_mpf_t());
    if (fv!=iv) id=TkT::FLOAT_LIT;
    break;
}
OPC(POW_EQ) OPC(POW) {
    gmp::ibig mod("18446744073709551615");
    mpz_powm(iv.get_mpz_t(), LD(i)->get_mpz_t(), RD(i)->get_mpz_t(), mod.get_mpz_t());
    break;
}
OPC(HNDRP_EQ) OPC(HNDRP)
    iv= *LD(i) % *RD(i);
    break;
OPC(ASSEGN)
    LEX.pwrg("assign an lvalue", exp->opl->loc, exp->opl->len);
    iv= *RD(i);
    break;
// OPC(LSH_EQ) OPC(LSH)
//     iv= *LD(i) << *RD(i);
//     break;
// OPC(RSH_EQ) OPC(RSH)
//     iv= *LD(i) >> *RD(i);
//     break;

OPC(AND_EQ) OPC(AND)
    iv= int(*LD(i) && *RD(i));
    break;
OPC(OR_EQ) OPC(OR)
    iv= int(*LD(i) || *RD(i));
    break;
OPC(AND_SIGN_EQ) OPC(AND_SIGN)
    iv= *LD(i) & *RD(i);
    break;
OPC(OR_SIGN_EQ) OPC(OR_SIGN)
    iv= *LD(i) | *RD(i);
    break;
OPC(XOR_EQ) OPC(XOR)
    iv= *LD(i) ^ *RD(i);
    break;
OPC(EQ)
    iv= int(*LD(i) == *RD(i));
    break;
OPC(NOT_EQ)
    iv= int(*LD(i) != *RD(i));
    break;
OPC(LESS)
    iv= int(*LD(i) < *RD(i));
    break;
OPC(GREADER)
    iv= int(*LD(i) > *RD(i));
    break;
OPC(LEQ)
    iv= int(*LD(i) <= *RD(i));
    break;
OPC(GEQ)
    iv= int(*LD(i) >= *RD(i));
    break;
default:
    LEX.perr("unsupported operation", exp->loc, exp->len);
    return false;
