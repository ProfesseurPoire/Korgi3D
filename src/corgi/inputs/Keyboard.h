#pragma once

namespace corgi
{
    enum class Key : int
    {
        A = 4,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Left_ctrl = 26,
        Del = 32,
        Return = 28,
        F5 = 29,
        Escape = 41
    };

    class Window;

    class Keyboard
    {
    public:

        friend class Inputs;
        friend class Window;

        [[nodiscard]] bool key(Key k) const;
        [[nodiscard]] bool key_down(Key k)const;
        [[nodiscard]] bool key_up(Key k)const;

        void update();

    private:

        bool _keys[349];
        bool _keys_down[349];
        bool _keys_up[349];

        Keyboard();
    };
}