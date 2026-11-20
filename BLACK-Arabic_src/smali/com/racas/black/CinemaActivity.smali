.class public Lcom/racas/black/CinemaActivity;
.super Landroid/app/Activity;
.source "CinemaActivity.java"


# static fields
.field private static final FILES:[Ljava/lang/String;

.field private static final TITLES:[Ljava/lang/String;


# instance fields
.field private bar:Landroid/widget/LinearLayout;

.field private dragging:Z

.field private face:Landroid/graphics/Typeface;

.field private final handler:Landroid/os/Handler;

.field private play:Landroid/widget/Button;

.field private position:I

.field private resumePlayback:Z

.field private root:Landroid/widget/FrameLayout;

.field private seek:Landroid/widget/SeekBar;

.field private selected:I

.field private final tick:Ljava/lang/Runnable;

.field private time:Landroid/widget/TextView;

.field private video:Landroid/widget/VideoView;


# direct methods
.method static constructor <clinit>()V
    .locals 10

    .line 19
    const-string v7, "17_n"

    const-string v8, "18_n"

    const-string v0, "03_n"

    const-string v1, "05_n"

    const-string v2, "07_n"

    const-string v3, "09_n"

    const-string v4, "11_n"

    const-string v5, "13_n"

    const-string v6, "15_n"

    filled-new-array/range {v0 .. v8}, [Ljava/lang/String;

    move-result-object v0

    sput-object v0, Lcom/racas/black/CinemaActivity;->FILES:[Ljava/lang/String;

    .line 20
    const-string v8, "\u0627\u0644\u0645\u0633\u062a\u0648\u0649"

    const-string v9, "\u0627\u0644\u0642\u064a\u0627\u062f\u0629"

    const-string v1, "\u0627\u0644\u0645\u0648\u062c\u0629 \u0627\u0644\u0633\u0627\u0628\u0639\u0629"

    const-string v2, "\u0627\u0644\u0647\u062f\u0641"

    const-string v3, "\u0627\u0644\u0623\u0645\u0631\u064a\u0643\u064a"

    const-string v4, "\u0627\u0644\u0645\u0635\u0627\u062f\u0631"

    const-string v5, "\u0633\u0648\u0644\u0648\u0645\u0648\u0646"

    const-string v6, "\u0641\u0627\u0644\u064a\u0646\u0633\u064a\u0648"

    const-string v7, "\u0627\u0644\u0639\u0645\u0644\u064a\u0627\u062a"

    filled-new-array/range {v1 .. v9}, [Ljava/lang/String;

    move-result-object v0

    sput-object v0, Lcom/racas/black/CinemaActivity;->TITLES:[Ljava/lang/String;

    return-void
.end method

.method public constructor <init>()V
    .locals 1

    .line 18
    invoke-direct {p0}, Landroid/app/Activity;-><init>()V

    .line 21
    new-instance v0, Landroid/os/Handler;

    invoke-direct {v0}, Landroid/os/Handler;-><init>()V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->handler:Landroid/os/Handler;

    .line 29
    const/4 v0, -0x1

    iput v0, p0, Lcom/racas/black/CinemaActivity;->selected:I

    .line 137
    new-instance v0, Lcom/racas/black/CinemaActivity$13;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$13;-><init>(Lcom/racas/black/CinemaActivity;)V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->tick:Ljava/lang/Runnable;

    return-void
.end method

.method static synthetic access$000(Lcom/racas/black/CinemaActivity;II)V
    .locals 0

    .line 18
    invoke-direct {p0, p1, p2}, Lcom/racas/black/CinemaActivity;->startMovie(II)V

    return-void
.end method

.method static synthetic access$100(Lcom/racas/black/CinemaActivity;)V
    .locals 0

    .line 18
    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->showList()V

    return-void
.end method

.method static synthetic access$1000(Lcom/racas/black/CinemaActivity;)Landroid/os/Handler;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->handler:Landroid/os/Handler;

    return-object p0
.end method

.method static synthetic access$1100(I)Ljava/lang/String;
    .locals 0

    .line 18
    invoke-static {p0}, Lcom/racas/black/CinemaActivity;->clock(I)Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method static synthetic access$1200(Lcom/racas/black/CinemaActivity;)Landroid/widget/TextView;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->time:Landroid/widget/TextView;

    return-object p0
.end method

.method static synthetic access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    return-object p0
.end method

.method static synthetic access$300(Lcom/racas/black/CinemaActivity;)Landroid/widget/Button;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->play:Landroid/widget/Button;

    return-object p0
.end method

.method static synthetic access$400(Lcom/racas/black/CinemaActivity;)I
    .locals 0

    .line 18
    iget p0, p0, Lcom/racas/black/CinemaActivity;->selected:I

    return p0
.end method

.method static synthetic access$500()[Ljava/lang/String;
    .locals 1

    .line 18
    sget-object v0, Lcom/racas/black/CinemaActivity;->FILES:[Ljava/lang/String;

    return-object v0
.end method

.method static synthetic access$600(Lcom/racas/black/CinemaActivity;)Z
    .locals 0

    .line 18
    iget-boolean p0, p0, Lcom/racas/black/CinemaActivity;->dragging:Z

    return p0
.end method

.method static synthetic access$602(Lcom/racas/black/CinemaActivity;Z)Z
    .locals 0

    .line 18
    iput-boolean p1, p0, Lcom/racas/black/CinemaActivity;->dragging:Z

    return p1
.end method

.method static synthetic access$700(Lcom/racas/black/CinemaActivity;)Landroid/widget/LinearLayout;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    return-object p0
.end method

.method static synthetic access$800(Lcom/racas/black/CinemaActivity;)Landroid/widget/SeekBar;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->seek:Landroid/widget/SeekBar;

    return-object p0
.end method

.method static synthetic access$900(Lcom/racas/black/CinemaActivity;)Ljava/lang/Runnable;
    .locals 0

    .line 18
    iget-object p0, p0, Lcom/racas/black/CinemaActivity;->tick:Ljava/lang/Runnable;

    return-object p0
.end method

.method private button(Ljava/lang/String;)Landroid/widget/Button;
    .locals 3

    .line 57
    new-instance v0, Landroid/widget/Button;

    invoke-direct {v0, p0}, Landroid/widget/Button;-><init>(Landroid/content/Context;)V

    .line 58
    invoke-virtual {v0, p1}, Landroid/widget/Button;->setText(Ljava/lang/CharSequence;)V

    iget-object p1, p0, Lcom/racas/black/CinemaActivity;->face:Landroid/graphics/Typeface;

    invoke-virtual {v0, p1}, Landroid/widget/Button;->setTypeface(Landroid/graphics/Typeface;)V

    const/4 p1, -0x1

    invoke-virtual {v0, p1}, Landroid/widget/Button;->setTextColor(I)V

    const/high16 p1, 0x41880000    # 17.0f

    invoke-virtual {v0, p1}, Landroid/widget/Button;->setTextSize(F)V

    .line 59
    const/16 p1, 0x31

    const/16 v1, 0x2a

    const/16 v2, 0x1d

    invoke-static {v2, p1, v1}, Landroid/graphics/Color;->rgb(III)I

    move-result p1

    invoke-virtual {v0, p1}, Landroid/widget/Button;->setBackgroundColor(I)V

    .line 60
    return-object v0
.end method

.method private static clock(I)Ljava/lang/String;
    .locals 2

    .line 136
    sget-object v0, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    const v1, 0xea60

    div-int v1, p0, v1

    invoke-static {v1}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;

    move-result-object v1

    div-int/lit16 p0, p0, 0x3e8

    rem-int/lit8 p0, p0, 0x3c

    invoke-static {p0}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;

    move-result-object p0

    filled-new-array {v1, p0}, [Ljava/lang/Object;

    move-result-object p0

    const-string v1, "%d:%02d"

    invoke-static {v0, v1, p0}, Ljava/lang/String;->format(Ljava/util/Locale;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object p0

    return-object p0
.end method

.method private dp(I)I
    .locals 1

    .line 49
    int-to-float p1, p1

    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getResources()Landroid/content/res/Resources;

    move-result-object v0

    invoke-virtual {v0}, Landroid/content/res/Resources;->getDisplayMetrics()Landroid/util/DisplayMetrics;

    move-result-object v0

    iget v0, v0, Landroid/util/DisplayMetrics;->density:F

    mul-float/2addr p1, v0

    invoke-static {p1}, Ljava/lang/Math;->round(F)I

    move-result p1

    return p1
.end method

.method public static open(Landroid/app/Activity;)V
    .locals 1

    .line 33
    new-instance v0, Lcom/racas/black/CinemaActivity$1;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$1;-><init>(Landroid/app/Activity;)V

    invoke-virtual {p0, v0}, Landroid/app/Activity;->runOnUiThread(Ljava/lang/Runnable;)V

    .line 38
    return-void
.end method

.method private showList()V
    .locals 11

    .line 68
    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->stopVideo()V

    const/4 v0, -0x1

    iput v0, p0, Lcom/racas/black/CinemaActivity;->selected:I

    .line 69
    new-instance v1, Landroid/widget/LinearLayout;

    invoke-direct {v1, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    const/4 v2, 0x1

    invoke-virtual {v1, v2}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 70
    const/16 v3, 0x16

    invoke-direct {p0, v3}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v4

    const/16 v5, 0xa

    invoke-direct {p0, v5}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v6

    invoke-direct {p0, v3}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v3

    invoke-direct {p0, v5}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v5

    invoke-virtual {v1, v4, v6, v3, v5}, Landroid/widget/LinearLayout;->setPadding(IIII)V

    const/16 v3, 0xe

    const/16 v4, 0x8

    const/16 v5, 0x10

    invoke-static {v4, v5, v3}, Landroid/graphics/Color;->rgb(III)I

    move-result v3

    invoke-virtual {v1, v3}, Landroid/widget/LinearLayout;->setBackgroundColor(I)V

    .line 71
    const-string v3, "\u0627\u0644\u0645\u0634\u0627\u0647\u062f \u0627\u0644\u0633\u064a\u0646\u0645\u0627\u0626\u064a\u0629"

    const/16 v4, 0x19

    invoke-direct {p0, v3, v4}, Lcom/racas/black/CinemaActivity;->text(Ljava/lang/String;I)Landroid/widget/TextView;

    move-result-object v3

    invoke-virtual {v1, v3}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 72
    const-string v3, "\u0627\u062e\u062a\u0631 \u0623\u064a \u0645\u0634\u0647\u062f \u0644\u0645\u0634\u0627\u0647\u062f\u062a\u0647 \u2014 \u0644\u0627 \u062a\u062d\u062a\u0627\u062c \u0625\u0644\u0649 \u0627\u0644\u062a\u0642\u062f\u0645 \u0641\u064a \u0627\u0644\u0642\u0635\u0629"

    invoke-direct {p0, v3, v5}, Lcom/racas/black/CinemaActivity;->text(Ljava/lang/String;I)Landroid/widget/TextView;

    move-result-object v3

    invoke-virtual {v1, v3}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 73
    new-instance v3, Landroid/widget/ScrollView;

    invoke-direct {v3, p0}, Landroid/widget/ScrollView;-><init>(Landroid/content/Context;)V

    .line 74
    new-instance v4, Landroid/widget/LinearLayout;

    invoke-direct {v4, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    invoke-virtual {v4, v2}, Landroid/widget/LinearLayout;->setOrientation(I)V

    .line 75
    const/4 v2, 0x0

    move v5, v2

    :goto_0
    sget-object v6, Lcom/racas/black/CinemaActivity;->FILES:[Ljava/lang/String;

    array-length v6, v6

    if-ge v5, v6, :cond_0

    .line 76
    nop

    .line 77
    sget-object v6, Ljava/util/Locale;->ROOT:Ljava/util/Locale;

    add-int/lit8 v7, v5, 0x1

    invoke-static {v7}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;

    move-result-object v8

    sget-object v9, Lcom/racas/black/CinemaActivity;->TITLES:[Ljava/lang/String;

    aget-object v9, v9, v5

    filled-new-array {v8, v9}, [Ljava/lang/Object;

    move-result-object v8

    const-string v9, "%d  \u2014  %s"

    invoke-static {v6, v9, v8}, Ljava/lang/String;->format(Ljava/util/Locale;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v6

    invoke-direct {p0, v6}, Lcom/racas/black/CinemaActivity;->button(Ljava/lang/String;)Landroid/widget/Button;

    move-result-object v6

    .line 78
    new-instance v8, Landroid/widget/LinearLayout$LayoutParams;

    const/16 v9, 0x34

    invoke-direct {p0, v9}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v9

    invoke-direct {v8, v0, v9}, Landroid/widget/LinearLayout$LayoutParams;-><init>(II)V

    const/4 v9, 0x4

    invoke-direct {p0, v9}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v10

    invoke-direct {p0, v9}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v9

    invoke-virtual {v8, v2, v10, v2, v9}, Landroid/widget/LinearLayout$LayoutParams;->setMargins(IIII)V

    .line 79
    invoke-virtual {v4, v6, v8}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 80
    new-instance v8, Lcom/racas/black/CinemaActivity$2;

    invoke-direct {v8, p0, v5}, Lcom/racas/black/CinemaActivity$2;-><init>(Lcom/racas/black/CinemaActivity;I)V

    invoke-virtual {v6, v8}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 75
    move v5, v7

    goto :goto_0

    .line 82
    :cond_0
    invoke-virtual {v3, v4}, Landroid/widget/ScrollView;->addView(Landroid/view/View;)V

    new-instance v4, Landroid/widget/LinearLayout$LayoutParams;

    const/high16 v5, 0x3f800000    # 1.0f

    invoke-direct {v4, v0, v2, v5}, Landroid/widget/LinearLayout$LayoutParams;-><init>(IIF)V

    invoke-virtual {v1, v3, v4}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 83
    const-string v2, "\u0627\u0644\u0639\u0648\u062f\u0629 \u0625\u0644\u0649 \u0627\u0644\u0644\u0639\u0628\u0629"

    invoke-direct {p0, v2}, Lcom/racas/black/CinemaActivity;->button(Ljava/lang/String;)Landroid/widget/Button;

    move-result-object v2

    new-instance v3, Landroid/widget/LinearLayout$LayoutParams;

    const/16 v4, 0x30

    invoke-direct {p0, v4}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v4

    invoke-direct {v3, v0, v4}, Landroid/widget/LinearLayout$LayoutParams;-><init>(II)V

    invoke-virtual {v1, v2, v3}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 84
    new-instance v0, Lcom/racas/black/CinemaActivity$3;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$3;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v2, v0}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 85
    invoke-virtual {p0, v1}, Lcom/racas/black/CinemaActivity;->setContentView(Landroid/view/View;)V

    .line 86
    return-void
.end method

.method private startMovie(II)V
    .locals 7

    .line 89
    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->stopVideo()V

    iput p1, p0, Lcom/racas/black/CinemaActivity;->selected:I

    iput p2, p0, Lcom/racas/black/CinemaActivity;->position:I

    .line 90
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getWindow()Landroid/view/Window;

    move-result-object v0

    const/16 v1, 0x80

    invoke-virtual {v0, v1}, Landroid/view/Window;->addFlags(I)V

    .line 91
    new-instance v0, Landroid/widget/FrameLayout;

    invoke-direct {v0, p0}, Landroid/widget/FrameLayout;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    const/high16 v1, -0x1000000

    invoke-virtual {v0, v1}, Landroid/widget/FrameLayout;->setBackgroundColor(I)V

    .line 92
    new-instance v0, Landroid/widget/VideoView;

    invoke-direct {v0, p0}, Landroid/widget/VideoView;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    .line 93
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    iget-object v1, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v2, Landroid/widget/FrameLayout$LayoutParams;

    const/16 v3, 0x11

    const/4 v4, -0x1

    invoke-direct {v2, v4, v4, v3}, Landroid/widget/FrameLayout$LayoutParams;-><init>(III)V

    invoke-virtual {v0, v1, v2}, Landroid/widget/FrameLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 94
    new-instance v0, Landroid/widget/LinearLayout;

    invoke-direct {v0, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    const/4 v1, 0x1

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setOrientation(I)V

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    const v1, -0x22f7eff2

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setBackgroundColor(I)V

    .line 95
    new-instance v0, Landroid/widget/LinearLayout;

    invoke-direct {v0, p0}, Landroid/widget/LinearLayout;-><init>(Landroid/content/Context;)V

    const/16 v1, 0x10

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setGravity(I)V

    .line 96
    const-string v2, "\u0642\u0627\u0626\u0645\u0629 \u0627\u0644\u0645\u0634\u0627\u0647\u062f"

    invoke-direct {p0, v2}, Lcom/racas/black/CinemaActivity;->button(Ljava/lang/String;)Landroid/widget/Button;

    move-result-object v2

    invoke-virtual {v0, v2}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 97
    new-instance v3, Lcom/racas/black/CinemaActivity$4;

    invoke-direct {v3, p0}, Lcom/racas/black/CinemaActivity$4;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v2, v3}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 98
    const-string v2, "\u0625\u064a\u0642\u0627\u0641 \u0645\u0624\u0642\u062a"

    invoke-direct {p0, v2}, Lcom/racas/black/CinemaActivity;->button(Ljava/lang/String;)Landroid/widget/Button;

    move-result-object v2

    iput-object v2, p0, Lcom/racas/black/CinemaActivity;->play:Landroid/widget/Button;

    iget-object v2, p0, Lcom/racas/black/CinemaActivity;->play:Landroid/widget/Button;

    invoke-virtual {v0, v2}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 99
    iget-object v2, p0, Lcom/racas/black/CinemaActivity;->play:Landroid/widget/Button;

    new-instance v3, Lcom/racas/black/CinemaActivity$5;

    invoke-direct {v3, p0}, Lcom/racas/black/CinemaActivity$5;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v2, v3}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 104
    sget-object v2, Lcom/racas/black/CinemaActivity;->TITLES:[Ljava/lang/String;

    aget-object v2, v2, p1

    invoke-direct {p0, v2, v1}, Lcom/racas/black/CinemaActivity;->text(Ljava/lang/String;I)Landroid/widget/TextView;

    move-result-object v1

    iput-object v1, p0, Lcom/racas/black/CinemaActivity;->time:Landroid/widget/TextView;

    iget-object v1, p0, Lcom/racas/black/CinemaActivity;->time:Landroid/widget/TextView;

    new-instance v2, Landroid/widget/LinearLayout$LayoutParams;

    const/high16 v3, 0x3f800000    # 1.0f

    const/4 v5, 0x0

    const/4 v6, -0x2

    invoke-direct {v2, v5, v6, v3}, Landroid/widget/LinearLayout$LayoutParams;-><init>(IIF)V

    invoke-virtual {v0, v1, v2}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 105
    const-string v1, "\u0627\u0644\u062a\u0627\u0644\u064a"

    invoke-direct {p0, v1}, Lcom/racas/black/CinemaActivity;->button(Ljava/lang/String;)Landroid/widget/Button;

    move-result-object v1

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 106
    new-instance v2, Lcom/racas/black/CinemaActivity$6;

    invoke-direct {v2, p0}, Lcom/racas/black/CinemaActivity$6;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v1, v2}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 107
    iget-object v1, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    invoke-virtual {v1, v0}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 108
    new-instance v0, Landroid/widget/SeekBar;

    invoke-direct {v0, p0}, Landroid/widget/SeekBar;-><init>(Landroid/content/Context;)V

    iput-object v0, p0, Lcom/racas/black/CinemaActivity;->seek:Landroid/widget/SeekBar;

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    iget-object v1, p0, Lcom/racas/black/CinemaActivity;->seek:Landroid/widget/SeekBar;

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->addView(Landroid/view/View;)V

    .line 109
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->seek:Landroid/widget/SeekBar;

    new-instance v1, Lcom/racas/black/CinemaActivity$7;

    invoke-direct {v1, p0}, Lcom/racas/black/CinemaActivity$7;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v0, v1}, Landroid/widget/SeekBar;->setOnSeekBarChangeListener(Landroid/widget/SeekBar$OnSeekBarChangeListener;)V

    .line 114
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    iget-object v1, p0, Lcom/racas/black/CinemaActivity;->bar:Landroid/widget/LinearLayout;

    new-instance v2, Landroid/widget/FrameLayout$LayoutParams;

    const/16 v3, 0x50

    invoke-direct {v2, v4, v6, v3}, Landroid/widget/FrameLayout$LayoutParams;-><init>(III)V

    invoke-virtual {v0, v1, v2}, Landroid/widget/FrameLayout;->addView(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V

    .line 115
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    new-instance v1, Lcom/racas/black/CinemaActivity$8;

    invoke-direct {v1, p0}, Lcom/racas/black/CinemaActivity$8;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v0, v1}, Landroid/widget/FrameLayout;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 116
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v1, Lcom/racas/black/CinemaActivity$9;

    invoke-direct {v1, p0}, Lcom/racas/black/CinemaActivity$9;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {v0, v1}, Landroid/widget/VideoView;->setOnTouchListener(Landroid/view/View$OnTouchListener;)V

    .line 120
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->root:Landroid/widget/FrameLayout;

    invoke-virtual {p0, v0}, Lcom/racas/black/CinemaActivity;->setContentView(Landroid/view/View;)V

    .line 121
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v1, Lcom/racas/black/CinemaActivity$10;

    invoke-direct {v1, p0, p2}, Lcom/racas/black/CinemaActivity$10;-><init>(Lcom/racas/black/CinemaActivity;I)V

    invoke-virtual {v0, v1}, Landroid/widget/VideoView;->setOnPreparedListener(Landroid/media/MediaPlayer$OnPreparedListener;)V

    .line 126
    iget-object p2, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v0, Lcom/racas/black/CinemaActivity$11;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$11;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {p2, v0}, Landroid/widget/VideoView;->setOnCompletionListener(Landroid/media/MediaPlayer$OnCompletionListener;)V

    .line 127
    iget-object p2, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v0, Lcom/racas/black/CinemaActivity$12;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$12;-><init>(Lcom/racas/black/CinemaActivity;)V

    invoke-virtual {p2, v0}, Landroid/widget/VideoView;->setOnErrorListener(Landroid/media/MediaPlayer$OnErrorListener;)V

    .line 132
    :try_start_0
    iget-object p2, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    new-instance v0, Ljava/lang/StringBuilder;

    invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V

    const-string v1, "android.resource://"

    invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getPackageName()Ljava/lang/String;

    move-result-object v1

    invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    const-string v1, "/raw/scene_"

    invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    sget-object v1, Lcom/racas/black/CinemaActivity;->FILES:[Ljava/lang/String;

    aget-object p1, v1, p1

    invoke-virtual {v0, p1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object p1

    invoke-virtual {p1}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object p1

    invoke-static {p1}, Landroid/net/Uri;->parse(Ljava/lang/String;)Landroid/net/Uri;

    move-result-object p1

    invoke-virtual {p2, p1}, Landroid/widget/VideoView;->setVideoURI(Landroid/net/Uri;)V
    :try_end_0
    .catch Ljava/lang/RuntimeException; {:try_start_0 .. :try_end_0} :catch_0

    .line 133
    goto :goto_0

    :catch_0
    move-exception p1

    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->showList()V

    .line 134
    :goto_0
    return-void
.end method

.method private stopVideo()V
    .locals 2

    .line 63
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->handler:Landroid/os/Handler;

    const/4 v1, 0x0

    invoke-virtual {v0, v1}, Landroid/os/Handler;->removeCallbacksAndMessages(Ljava/lang/Object;)V

    .line 64
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    if-eqz v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    invoke-virtual {v0}, Landroid/widget/VideoView;->stopPlayback()V

    iput-object v1, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    .line 65
    :cond_0
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getWindow()Landroid/view/Window;

    move-result-object v0

    const/16 v1, 0x80

    invoke-virtual {v0, v1}, Landroid/view/Window;->clearFlags(I)V

    .line 66
    return-void
.end method

.method private text(Ljava/lang/String;I)Landroid/widget/TextView;
    .locals 3

    .line 51
    new-instance v0, Landroid/widget/TextView;

    invoke-direct {v0, p0}, Landroid/widget/TextView;-><init>(Landroid/content/Context;)V

    .line 52
    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    const/4 p1, -0x1

    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setTextColor(I)V

    int-to-float p1, p2

    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setTextSize(F)V

    .line 53
    iget-object p1, p0, Lcom/racas/black/CinemaActivity;->face:Landroid/graphics/Typeface;

    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setTypeface(Landroid/graphics/Typeface;)V

    const/16 p1, 0x11

    invoke-virtual {v0, p1}, Landroid/widget/TextView;->setGravity(I)V

    const/16 p1, 0xa

    invoke-direct {p0, p1}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result p2

    const/4 v1, 0x6

    invoke-direct {p0, v1}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v2

    invoke-direct {p0, p1}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result p1

    invoke-direct {p0, v1}, Lcom/racas/black/CinemaActivity;->dp(I)I

    move-result v1

    invoke-virtual {v0, p2, v2, p1, v1}, Landroid/widget/TextView;->setPadding(IIII)V

    .line 54
    return-object v0
.end method


# virtual methods
.method public onBackPressed()V
    .locals 1

    .line 143
    iget v0, p0, Lcom/racas/black/CinemaActivity;->selected:I

    if-ltz v0, :cond_0

    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->showList()V

    goto :goto_0

    :cond_0
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->finish()V

    :goto_0
    return-void
.end method

.method public onCreate(Landroid/os/Bundle;)V
    .locals 1

    .line 41
    invoke-super {p0, p1}, Landroid/app/Activity;->onCreate(Landroid/os/Bundle;)V

    .line 42
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getWindow()Landroid/view/Window;

    move-result-object p1

    const/16 v0, 0x400

    invoke-virtual {p1, v0, v0}, Landroid/view/Window;->setFlags(II)V

    .line 43
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getWindow()Landroid/view/Window;

    move-result-object p1

    invoke-virtual {p1}, Landroid/view/Window;->getDecorView()Landroid/view/View;

    move-result-object p1

    const/16 v0, 0x1706

    invoke-virtual {p1, v0}, Landroid/view/View;->setSystemUiVisibility(I)V

    .line 44
    :try_start_0
    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getAssets()Landroid/content/res/AssetManager;

    move-result-object p1

    const-string v0, "thmanyah-Medium.otf"

    invoke-static {p1, v0}, Landroid/graphics/Typeface;->createFromAsset(Landroid/content/res/AssetManager;Ljava/lang/String;)Landroid/graphics/Typeface;

    move-result-object p1

    iput-object p1, p0, Lcom/racas/black/CinemaActivity;->face:Landroid/graphics/Typeface;
    :try_end_0
    .catch Ljava/lang/RuntimeException; {:try_start_0 .. :try_end_0} :catch_0

    .line 45
    goto :goto_0

    :catch_0
    move-exception p1

    sget-object p1, Landroid/graphics/Typeface;->DEFAULT:Landroid/graphics/Typeface;

    iput-object p1, p0, Lcom/racas/black/CinemaActivity;->face:Landroid/graphics/Typeface;

    .line 46
    :goto_0
    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->showList()V

    .line 47
    return-void
.end method

.method protected onDestroy()V
    .locals 0

    .line 152
    invoke-direct {p0}, Lcom/racas/black/CinemaActivity;->stopVideo()V

    invoke-super {p0}, Landroid/app/Activity;->onDestroy()V

    return-void
.end method

.method protected onPause()V
    .locals 1

    .line 145
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    if-eqz v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    invoke-virtual {v0}, Landroid/widget/VideoView;->getCurrentPosition()I

    move-result v0

    iput v0, p0, Lcom/racas/black/CinemaActivity;->position:I

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    invoke-virtual {v0}, Landroid/widget/VideoView;->isPlaying()Z

    move-result v0

    iput-boolean v0, p0, Lcom/racas/black/CinemaActivity;->resumePlayback:Z

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    invoke-virtual {v0}, Landroid/widget/VideoView;->pause()V

    .line 146
    :cond_0
    invoke-super {p0}, Landroid/app/Activity;->onPause()V

    .line 147
    return-void
.end method

.method protected onResume()V
    .locals 2

    .line 149
    invoke-super {p0}, Landroid/app/Activity;->onResume()V

    invoke-virtual {p0}, Lcom/racas/black/CinemaActivity;->getWindow()Landroid/view/Window;

    move-result-object v0

    invoke-virtual {v0}, Landroid/view/Window;->getDecorView()Landroid/view/View;

    move-result-object v0

    const/16 v1, 0x1706

    invoke-virtual {v0, v1}, Landroid/view/View;->setSystemUiVisibility(I)V

    .line 150
    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    if-eqz v0, :cond_0

    iget-boolean v0, p0, Lcom/racas/black/CinemaActivity;->resumePlayback:Z

    if-eqz v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    iget v1, p0, Lcom/racas/black/CinemaActivity;->position:I

    invoke-virtual {v0, v1}, Landroid/widget/VideoView;->seekTo(I)V

    iget-object v0, p0, Lcom/racas/black/CinemaActivity;->video:Landroid/widget/VideoView;

    invoke-virtual {v0}, Landroid/widget/VideoView;->start()V

    const/4 v0, 0x0

    iput-boolean v0, p0, Lcom/racas/black/CinemaActivity;->resumePlayback:Z

    .line 151
    :cond_0
    return-void
.end method
