#pragma once

template<typename T>
void Release(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}
