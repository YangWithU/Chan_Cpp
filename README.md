# Chan_Cpp
使用c++对golang channel的实现

## 编译运行

1. 克隆到本地
```shell
git clone --depth 1 https://github.com/YangWithU/Chan_Cpp.git
```
2. CMake 构建项目

```shell
mkdir build
cd build
cmake ..
```
3. 使用 make 或 ninja进行编译

```shell
make
```
4. 运行

```shell
./chan_cpp
```

## features

- [x] 基础 Go Channel 类型
- [x] Buffered Channel
- [x] 重载 `<<` 进行 Channel 对象存储和提取
- [x] 基本 Select 语句
- [x] 向 Channel 插入 Select 语句
- [x] 乱序 Case 执行 Select 语句
- [x] Channel 迭代器
- [x] Close() 函数

## examples

### 无缓冲 Channel

```c++
Chan<int> ch;
// 插入
int i = 2;
ch << 1; //or
i >> ch;
// 提取
i << ch; //or
int y;
ch >> y
```

### 有缓冲 Channel

```c++
Chan<int, 5> multi;

thread([&]() {
	multi << 1 << 2 << 3 << 4 << 5;
	this_thread::sleep_for(chrono::milliseconds(500));
	Close(multi);
}).detach();

for(auto& x : multi) {
	cout << x << endl;
}
```

### Select

```c++
void fibonacci(Chan<int>& c, Chan<int>& quit)
{
	int x=0, y = 1;
	for (bool go = true; go;)
	{
		Select
		{
			Case{c << x,[&]()
			{
				int t = x;
				x = y;
				y += t;
			}},
			Case{quit,[&](auto v) 
			{
				cout << "quit" << endl;
				go = false;
			}}
		};
	}
}

int main()
{
	Chan<int> c;
	Chan<int> quit;

	thread([&]()
	{
		for (size_t i = 0; i < 10; i++)
		{
			cout << c << endl;
		}
		quit << 0;
	}).detach();
	fibonacci(c, quit);
}
```