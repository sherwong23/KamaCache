#pragma once

namespace KamaCache
{

    template <typename Key, typename Value>
    class KICachePolicy
    {
        public:
            virtual ~KICachePolicy() {};

            //添加缓存接口
            virtual void put (Key key, Value value) = 0;
            virtual bool get (Key key, Value& value) = 0;
            virtual Value get(Key key) = 0;

    };
} // namespace KamaCache
