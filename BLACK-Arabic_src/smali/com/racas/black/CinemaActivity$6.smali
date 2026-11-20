.class Lcom/racas/black/CinemaActivity$6;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/view/View$OnClickListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->startMovie(II)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 106
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$6;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onClick(Landroid/view/View;)V
    .locals 2

    .line 106
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$6;->this$0:Lcom/racas/black/CinemaActivity;

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$6;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$400(Lcom/racas/black/CinemaActivity;)I

    move-result v0

    add-int/lit8 v0, v0, 0x1

    invoke-static {}, Lcom/racas/black/CinemaActivity;->access$500()[Ljava/lang/String;

    move-result-object v1

    array-length v1, v1

    rem-int/2addr v0, v1

    const/4 v1, 0x0

    invoke-static {p1, v0, v1}, Lcom/racas/black/CinemaActivity;->access$000(Lcom/racas/black/CinemaActivity;II)V

    return-void
.end method
