OPC(PLUS_EQ) OPC(PLUS) OPC(ADD_EQ) OPC(ADD) { // may overflow
    gmp::ibig iv;
    iv=i64(LD(c)) + i64(RD(c));
    usz req= mpz_sizeinbase(iv.get_mpz_t(), 2);
    if (req>sizeof(ch32)*8) {
        exp->id=TkT::NUM_LIT;
        Exp::operator delete(exp->opl=nullptr);
        Exp::operator delete(exp->opr=nullptr);
        exp->data.i=push_num(iv);
        return true;
    }
    result= ch32(iv.get_si());
}   break;

OPC(MINUS_EQ) OPC(MINUS) OPC(SUB_EQ) OPC(SUB)
    result= LD(c) - RD(c);
    break;

OPC(MUL_EQ) OPC(MUL) { // may overflow
    gmp::ibig iv;
    iv=i64(LD(c)) * i64(RD(c));
    usz req= mpz_sizeinbase(iv.get_mpz_t(), 2);
    if (req>sizeof(ch32)*8) {
        exp->id=TkT::NUM_LIT;
        Exp::operator delete(exp->opl=nullptr);
        Exp::operator delete(exp->opr=nullptr);
        exp->data.i=push_num(iv);
        return true;
    }
    result= ch32(iv.get_si());
}   break;

OPC(DIV_EQ) OPC(DIV)
    result= LD(c) / RD(c);
    break;

OPC(POW_EQ) OPC(POW) { // may overflow
    gmp::ibig iv;
    iv=std::pow(int(LD(c)), int(RD(c)));
    usz req= mpz_sizeinbase(iv.get_mpz_t(), 2);
    if (req>sizeof(ch32)*8) {
        exp->id=TkT::NUM_LIT;
        Exp::operator delete(exp->opl=nullptr);
        Exp::operator delete(exp->opr=nullptr);
        exp->data.i=push_num(iv);
        return true;
    }
    result= ch32(iv.get_si());
}   break;

OPC(HNDRP_EQ) OPC(HNDRP)
    result= LD(c) & RD(c);
    break;

OPC(ASSEGN)
    LEX.pwrg("assign an lvalue", exp->opl->loc, exp->opl->len);
    result= RD(c);
    break;

OPC(LSH_EQ) OPC(LSH)
    result= LD(c) << RD(c);
    break;

OPC(RSH_EQ) OPC(RSH)
    result= LD(c) >> RD(c);
    break;

OPC(AND_EQ) OPC(AND)
    result= LD(c) && RD(c);
    break;

OPC(OR_EQ) OPC(OR)
    result= LD(c) || RD(c);
    break;

OPC(AND_SIGN_EQ) OPC(AND_SIGN)
    result= LD(c) & RD(c);
    break;

OPC(OR_SIGN_EQ) OPC(OR_SIGN)
    result= LD(c) | RD(c);
    break;

OPC(XOR_EQ) OPC(XOR)
    result= LD(c) ^ RD(c);
    break;

OPC(EQ)
    result= LD(c) == RD(c);
    break;

OPC(NOT_EQ)
    result= LD(c) != RD(c);
    break;

OPC(LESS)
    result= LD(c) < RD(c);
    break;

OPC(GREADER)
    result= LD(c) > RD(c);
    break;

OPC(LEQ)
    result= LD(c) <= RD(c);
    break;

OPC(GEQ)
    result= LD(c) >= RD(c);
    break;

default:
    LEX.perr("unsupported operation", exp->loc, exp->len);
    return false;
